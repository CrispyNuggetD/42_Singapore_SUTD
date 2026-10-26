#!/usr/bin/env python3
"""Reset the personal desktop-login guard, optionally replacing terminal windows."""
import argparse
import fcntl
import importlib.util
import os
from pathlib import Path
import re
import signal
import shutil
import subprocess
import sys
import time
import tempfile
import workspace_restart as workspace


def session_state():
    if sys.platform=='darwin':
        return Path(os.environ.get('TMPDIR',tempfile.gettempdir()))/('42-login-'+str(os.getuid()))
    runtime = os.environ.get('XDG_RUNTIME_DIR', '')
    if not runtime or not Path(runtime).is_absolute():
        raise RuntimeError('XDG_RUNTIME_DIR is unavailable; run this in your desktop session.')
    session = os.environ.get('XDG_SESSION_ID', '')
    if not session:
        result = subprocess.run(['loginctl', 'list-sessions', '--no-legend'],
                                check=True, capture_output=True, text=True)
        session = next((parts[0] for line in result.stdout.splitlines()
                        if len(parts := line.split()) > 1 and parts[1] == str(os.getuid())), '')
    if not re.fullmatch(r'[A-Za-z0-9_-]+', session):
        raise RuntimeError('Could not identify the desktop session; no flags were changed.')
    return Path(runtime) / ('42-login-' + session)


def reload_marker():
    return Path.home() / '.cache/42-shell/reload-request'


def reset(check=False):
    state = session_state()
    if check:
        print('Would clear the dailylogin startup flag: ' + str(state / 'started'))
        print('Would request shell reloads; running jobs and unfinished input are kept.')
        return
    state.mkdir(parents=True, exist_ok=True, mode=0o700)
    # Use the same lock as 42-session-start; never remove its lock file.
    with (state / 'start.lock').open('a') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        (state / 'started').unlink(missing_ok=True)
    marker = reload_marker()
    marker.parent.mkdir(parents=True, exist_ok=True, mode=0o700)
    temporary = marker.with_name(marker.name + '.' + str(os.getpid()))
    temporary.write_text(str(time.time_ns()) + '\n')
    os.replace(temporary, marker)
    print('Dailylogin startup flag cleared. The next new terminal can run startup again.')
    print('Other existing Zsh terminals reload at their next prompt; jobs are not interrupted.')


def helper_paths():
    runtime = Path(os.environ.get('ZSHRC_RUNTIME_ROOT', str(Path.home() / '.local/share/42-shell')))
    closer = runtime / 'soft_quit_apps.py'
    launcher = Path.home() / '.local/bin/42-session-start'
    if not closer.is_file():
        raise RuntimeError('Terminal-close helper missing; run synczshrc first: ' + str(closer))
    if not launcher.is_file() or not os.access(launcher, os.X_OK) or sys.platform=='darwin':
        launcher=None
    if sys.platform!='darwin' and not os.environ.get('DISPLAY'):
        raise RuntimeError('No graphical desktop display; reopen a terminal yourself after resetdailylogin.')
    if launcher is None and sys.platform!='darwin' and not any(shutil.which(name) for name in ('gnome-terminal','xterm')):
        raise RuntimeError('Install a supported terminal launcher (GNOME Terminal or xterm) before relogin.')
    return closer, launcher


def tmux_command():
    command = ['tmux']
    if os.environ.get('DAILY_TMUX_SOCKET'):
        command += ['-S', os.environ['DAILY_TMUX_SOCKET']]
    return command


TERMINAL_EXECUTABLES = {
    'gnome-terminal-server', 'xterm', 'uxterm', 'konsole', 'kitty', 'alacritty',
    'xfce4-terminal', 'tilix', 'wezterm-gui', 'ghostty', 'foot', 'lxterminal',
    'qterminal', 'mate-terminal',
}


def find_active_jobs(rows, terminal_pids, pane_ttys, current_pid):
    processes = {row['pid']: row for row in rows}
    def ancestors(pid):
        seen = set()
        while pid in processes and pid not in seen:
            seen.add(pid)
            pid = processes[pid]['ppid']
        return seen
    caller = ancestors(current_pid)
    desktop_ttys = {row['tty'] for row in rows if row['tty'] != '?'
                    and ancestors(row['pid']) & terminal_pids}
    shells = {'zsh', 'bash', 'sh', 'dash', 'fish', 'ksh', 'tcsh', 'csh'}
    jobs = []
    for row in rows:
        pid, command = row['pid'], row['command']
        if (row['tty'] not in desktop_ttys | pane_ttys
                or not (ancestors(pid) & terminal_pids or row['tty'] in pane_ttys)
                or pid in caller or current_pid in ancestors(pid)
                or row['state'].startswith('Z') or command.startswith('tmux')):
            continue
        if command in shells and (pid == row['sid'] or row['ppid'] in terminal_pids
                or row['tty'] in pane_ttys and processes.get(row['ppid'],{}).get('tty')!=row['tty']) and not row['state'].startswith('R'):
            continue  # Sleeping session shell, with its jobs checked separately.
        jobs.append(row)
    return jobs


def active_terminal_jobs(closer):
    targets=terminal_targets(closer)
    rows=workspace.processes()
    _,pane_ttys=workspace.pane_processes(work_only=True)
    jobs=find_active_jobs(rows,{pid for pid,_ in targets},pane_ttys,os.getpid())
    jobs.extend(dict(command='LAN42 managed node',tty='background') for _ in workspace.managed_nodes())
    return jobs


def force_consent():
    if not sys.stdin.isatty():
        print('No interactive input; using normal terminal closure only.')
        return False
    print('A full re-login stops running terminal/tmux jobs and local LAN42 services. Unsaved progress may be lost.')
    while True:
        try:
            answer = input('Stop this running work, force-close blocked windows, and re-login? [y/N] ').strip().casefold()
        except EOFError:
            return False
        if answer in ('y', 'yes'):
            return True
        if answer in ('', 'n', 'no'):
            return False
        print('Please answer y or n.')


def terminal_identity(pid):
    try:
        directory = Path('/proc') / str(pid)
        if directory.stat().st_uid != os.getuid():
            return None
        executable = str((directory / 'exe').resolve())
        if Path(executable).name not in TERMINAL_EXECUTABLES:
            return None
        fields = (directory / 'stat').read_text().rpartition(')')[2].split()
        return (fields[19], executable) if fields[0] != 'Z' else None
    except (OSError, IndexError):
        return None


def terminal_targets(closer):
    if sys.platform=='darwin':return workspace.mac_terminal_targets()
    spec = importlib.util.spec_from_file_location('_relogin_terminal_discovery', closer)
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    display = module.X11()
    try:
        targets = []
        for app in display.apps().values():
            if app.matches(module.TERMINALS):
                identity = terminal_identity(app.pid)
                if identity:
                    targets.append((app.pid, identity))
        return targets
    finally:
        display.close()


def remaining_terminals(targets):
    # Recheck ownership, executable and start time before every signal.
    return [(pid, identity) for pid, identity in targets if terminal_identity(pid) == identity]


def wait_terminals(targets, timeout):
    deadline = time.monotonic() + timeout
    while remaining_terminals(targets) and time.monotonic() < deadline:
        time.sleep(0.1)
    return remaining_terminals(targets)


def force_terminals(targets):
    if sys.platform=='darwin':return workspace.mac_force_terminals(targets)
    if not targets:
        print('No verified terminal processes to force-close; manual closure is needed.', flush=True)
        return False
    print('Using the force-close permission granted before restart (terminal hosts only).', flush=True)
    for number, timeout in ((signal.SIGTERM, 3), (signal.SIGKILL, 2)):
        for pid, _ in remaining_terminals(targets):
            try:
                os.kill(pid, number)
            except ProcessLookupError:
                pass
        if not wait_terminals(targets, timeout):
            return True
    print('Some terminal hosts could not be closed; no new login was launched.', flush=True)
    return False


def worker(force=False):
    state = session_state()
    state.mkdir(parents=True, exist_ok=True, mode=0o700)
    with (state / 'relogin.lock').open('a') as lock:
        try:
            fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except BlockingIOError:
            print('A terminal relogin is already running.', flush=True)
            return 0
        closer, launcher = helper_paths()
        targets = terminal_targets(closer) if force else []
        # Stop the complete old workspace before creating any replacement panes.
        workspace.stop(force=force)
        print('Requesting normal closure of desktop terminal windows...', flush=True)
        try:
            result = subprocess.run([sys.executable, str(closer), '--terminals-only'], check=False,
                                    env=dict(os.environ, LEAVESCH_KEEP_APPS=''), timeout=5 if force else None)
            blocked = result.returncode != 0
        except subprocess.TimeoutExpired:
            blocked = True
        if blocked and force:
            blocked = not force_terminals(targets)
        if blocked:
            reset()
            print('Terminal closure needs attention; fresh login was not launched. '
                  'Finish the terminal prompts, then retry relogin or open a terminal yourself.', flush=True)
            return 1
        # Clear immediately before launch, after old shells have stopped opening windows.
        reset()
        print('Opening one fresh login terminal...', flush=True)
        workspace.launch_login(state,launcher)
        if not (state / 'started').exists():
            raise RuntimeError('Login launcher did not claim startup. Open a terminal yourself to retry.')
        print('Fresh login requested after stopping the old terminal/tmux workspace.', flush=True)
        return 0


def restart(check=False):
    state = session_state()
    closer, _ = helper_paths()  # Validate everything before closing any windows.
    try:
        jobs = active_terminal_jobs(closer)
    except Exception:
        jobs = None
    if jobs:
        print('Running terminal/workspace tasks: ' + ', '.join(sorted({job['command'] + ' (' + job['tty'] + ')' for job in jobs})))
    elif jobs is None:
        print('Active-job inspection unavailable; a full restart requires a reliable work check.')
    else:
        print('No active terminal/tmux work detected; no overwrite question is needed.')
    if check:
        reset(check=True)
        print('Would stop tmux jobs and managed LAN42 nodes, close terminals, then run fresh startup once.')
        sys.stdout.flush()
        subprocess.run([sys.executable, str(closer), '--check', '--terminals-only'], check=True,
                       env=dict(os.environ, LEAVESCH_KEEP_APPS=''))
        return
    if jobs is None:
        raise RuntimeError('Could not inspect running work; restart cancelled without closing anything.')
    force = bool(jobs) and force_consent()
    if jobs and not force:
        print('Re-login cancelled; running work was kept.')
        return
    # A detached worker survives closure of the invoking terminal and its tmux client.
    log_dir = Path(os.environ.get('XDG_STATE_HOME', str(Path.home() / '.local/state'))) / '42-shell'
    log_dir.mkdir(parents=True, exist_ok=True, mode=0o700)
    log_path = log_dir / 'relogin.log'
    descriptor = os.open(log_path, os.O_WRONLY | os.O_CREAT | os.O_APPEND, 0o600)
    state.mkdir(parents=True, exist_ok=True, mode=0o700)
    with os.fdopen(descriptor, 'a') as log:
        log.write('\n=== relogin ' + time.strftime('%Y-%m-%d %H:%M:%S') + ' ===\n')
        log.flush()
        command = [sys.executable, str(Path(__file__).resolve()), 'worker']
        if force:
            command.append('--force-terminals')
        subprocess.Popen(command,
                         stdin=subprocess.DEVNULL, stdout=log, stderr=log, start_new_session=True)
    print('Terminal restart scheduled. ' + ('Blocked terminal hosts may be force-closed.' if force
          else 'Normal close warnings are respected; force-close is disabled.'))
    print('The complete terminal/tmux workspace will be recreated; other desktop apps stay open.')
    print('If no new terminal appears, reopen one yourself. Details: ' + str(log_path))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('action', choices=('reset', 'restart', 'worker'))
    parser.add_argument('--check', action='store_true')
    parser.add_argument('--force-terminals', action='store_true', help=argparse.SUPPRESS)
    args = parser.parse_args()
    if args.force_terminals and args.action != 'worker':
        parser.error('Force-close permission is requested interactively by relogin.')
    try:
        if args.action == 'reset':
            reset(check=args.check)
        elif args.action == 'restart':
            restart(check=args.check)
        elif args.check:
            parser.error('Use restart --check to preview the worker.')
        else:
            return worker(force=args.force_terminals)
    except (OSError, RuntimeError, subprocess.SubprocessError) as error:
        print('Relogin needs attention: ' + str(error), file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
