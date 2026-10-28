#!/usr/bin/env python3
"""Toggle Linux push_swap runner/worker suspension in the selected tmux pane."""
import argparse
import fcntl
import json
import os
from pathlib import Path
import signal
import subprocess
import time


def processes():
    result = {}
    for path in Path('/proc').iterdir():
        if not path.name.isdecimal():
            continue
        try:
            if path.stat().st_uid != os.getuid():
                continue
            fields = (path / 'stat').read_text().rsplit(')', 1)[1].split()
            result[int(path.name)] = (int(fields[1]), fields[19])
        except (OSError, IndexError, ValueError):
            pass
    return result


def descendants(pid, table):
    found = [pid]
    for parent in found:
        found.extend(child for child, (ppid, _) in table.items() if ppid == parent and child not in found)
    return found


def is_runner(pid):
    try:
        args = (Path('/proc') / str(pid) / 'cmdline').read_bytes().split(b'\0')
        return (Path(os.fsdecode(args[0])).name.startswith('python')
                and len(args) > 1 and Path(os.fsdecode(args[1])).name == 'run_random_tests.py')
    except (OSError, IndexError):
        return False


def resume(saved):
    table = processes()
    for pid, started in reversed(saved):
        if pid in table and table[pid][1] == started:
            try:
                os.kill(pid, signal.SIGCONT)
            except ProcessLookupError:
                pass


def await_stopped(pid):
    deadline = time.monotonic() + 1
    while time.monotonic() < deadline:
        try:
            tasks = (Path('/proc') / str(pid) / 'task').iterdir()
            states = [p.joinpath('stat').read_text().rsplit(')', 1)[1].split()[0] for p in tasks]
            if all(state in ('T', 't', 'Z') for state in states):
                return
        except FileNotFoundError:
            return
        time.sleep(.005)
    raise RuntimeError('Could not safely suspend all worker threads; resumed stopped processes.')


def toggle(pane_pid, directory):
    directory.mkdir(parents=True, exist_ok=True, mode=0o700)
    with (directory / 'lock').open('a') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        table = processes()
        if pane_pid not in table:
            raise ValueError('Pane process has exited.')
        state = directory / ('%s-%s.json' % (pane_pid, table[pane_pid][1]))
        if state.exists():
            resume(json.loads(state.read_text()))
            state.unlink()
            return 'Solver resumed'
        roots = [pid for pid in descendants(pane_pid, table) if is_runner(pid)]
        if len(roots) != 1:
            raise ValueError('Select the pane running run_random_tests.py.')
        stopped = []
        pending = [roots[0]]
        try:
            # Freeze each parent before looking for its children, including
            # workers launched with start_new_session=True.
            for pid in pending:
                current = processes()
                if pid not in current:
                    continue
                if pid in table and current[pid][1] != table[pid][1]:
                    continue
                os.kill(pid, signal.SIGSTOP)
                stopped.append([pid, current[pid][1]])
                # Wait for the kernel to stop all threads before scanning births.
                await_stopped(pid)
                current = processes()
                pending.extend(child for child, (ppid, _) in current.items()
                               if ppid == pid and child not in pending)
            state.write_text(json.dumps(stopped))
            state.chmod(0o600)
        except BaseException:
            resume(stopped)
            state.unlink(missing_ok=True)
            raise
        return 'Solver paused (Ctrl-b then F8 to resume)'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--pane', required=True)
    args = parser.parse_args()
    try:
        pid = int(subprocess.check_output(['tmux', 'display-message', '-p', '-t', args.pane, '#{pane_pid}']))
        directory = Path(os.environ.get('XDG_STATE_HOME', str(Path.home() / '.local/state'))) / 'push-swap-pause'
        message = toggle(pid, directory)
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
        message = 'Solver pause: ' + str(error)
    subprocess.run(['tmux', 'display-message', '-t', args.pane, message], check=False)


if __name__ == '__main__':
    main()
