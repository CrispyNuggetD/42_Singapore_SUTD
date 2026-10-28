#!/usr/bin/env python3
"""Request timing-safe native runner pause/resume in the selected tmux pane."""
import argparse
import json
import os
from pathlib import Path
import signal
import subprocess


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


def toggle(pane_pid):
    table = processes()
    if pane_pid not in table:
        raise ValueError('Pane process has exited.')
    roots = [pid for pid in descendants(pane_pid, table) if is_runner(pid)]
    if len(roots) != 1:
        raise ValueError('Select the pane running run_random_tests.py.')
    pid = roots[0]
    path = Path(os.environ.get('XDG_STATE_HOME', str(Path.home() / '.local/state'))) / 'push-swap-control' / str(pid)
    try:
        capability = json.loads(path.read_text())
    except (OSError, ValueError):
        raise ValueError('Resume with the updated runner to enable timing-safe pause.') from None
    if capability.get('native_pause') != 1 or capability.get('started') != table[pid][1]:
        raise ValueError('Runner has no current timing-safe pause controls.')
    os.kill(pid, signal.SIGUSR1)
    return 'Solver pause/resume requested; see runner status'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--pane', required=True)
    args = parser.parse_args()
    try:
        pid = int(subprocess.check_output(['tmux', 'display-message', '-p', '-t', args.pane, '#{pane_pid}']))
        message = toggle(pid)
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
        message = 'Solver pause: ' + str(error)
    subprocess.run(['tmux', 'display-message', '-t', args.pane, message], check=False)


if __name__ == '__main__':
    main()
