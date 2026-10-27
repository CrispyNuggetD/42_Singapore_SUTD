#!/usr/bin/env python3
"""Bind your own private GitHub repo to screen-lock snapshots (GNOME Linux)."""
import argparse
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
from lock_backup import CONFIG, run

SERVICE = '42-lock-backup.service'

def status():
    try:
        config = json.loads(CONFIG.read_text())
        service = Path.home() / '.config/systemd/user' / SERVICE
        autostart = Path.home() / '.config/autostart/42-lock-backup.desktop'
        return bool(config.get('enabled') and config.get('private_confirmed') and service.is_file() and autostart.is_file())
    except (OSError, ValueError):
        return False

def install(source, remote, confirmed=False, chat_private=None):
    if not sys.platform.startswith('linux') or not os.environ.get('XDG_RUNTIME_DIR'):
        raise ValueError('Run setupprivatebackup inside your Linux desktop session')
    subprocess.run(['/usr/bin/python3', '-c', 'from gi.repository import Gio, GLib'], check=True)
    source = Path(source).expanduser().resolve(strict=True)
    top = Path(run(['git', '-C', str(source), 'rev-parse', '--show-toplevel']).decode().strip()).resolve()
    if top != source:
        raise ValueError('Choose the top-level school repository directory')
    match = re.fullmatch(r'git@github\.com:([A-Za-z0-9_.-]+)/([A-Za-z0-9_.-]+?)(?:\.git)?', remote)
    if not match:
        raise ValueError('Use SSH URL git@github.com:YOUR_USER/YOUR_PRIVATE_REPO.git')
    owner, name = match.groups()
    # Public repos are observable without authentication: never bind one.
    response = subprocess.run(['curl', '-sS', '--max-time', '20', '-o', '/dev/null', '-w', '%{http_code}',
        f'https://api.github.com/repos/{owner}/{name}'], check=True, capture_output=True, text=True).stdout
    if response == '200':
        raise ValueError('That repository is public; create a PRIVATE repository instead')
    if response != '404':
        raise ValueError('GitHub visibility check unavailable; try setup again later')
    if not confirmed:
        answer = input('Confirm you created this repository as PRIVATE (yes/no) [no]: ').strip().lower()
        if answer not in ('yes', 'y'):
            raise ValueError('Setup cancelled; private destination not confirmed')
    run(['git', 'ls-remote', remote])
    state = Path(os.environ.get('XDG_STATE_HOME', str(Path.home() / '.local/state'))) / '42-lock-checkpoint'
    config = dict(enabled=True, private_confirmed=True, source=str(source), paths=['.'],
                  storage=str(state), remote=remote, branch='wip/42-school')
    if chat_private:
        private = Path(chat_private).expanduser().resolve(strict=True)
        chat_remote = run(['git', '-C', str(private), 'remote', 'get-url', 'origin']).decode().strip()
        if private.name != '42_Singapore_SUTD_hnah_private':
            raise ValueError('Owner chat backups require the separate 42_Singapore_SUTD_hnah_private repo')
        config.update(chat_backup_private=str(private), chat_backup_remote=chat_remote)
    # Add one backup remote without changing public origin or an existing remote.
    existing = run(['git', '-C', str(source), 'remote']).decode().splitlines()
    if 'backup' in existing:
        if run(['git', '-C', str(source), 'remote', 'get-url', 'backup']).decode().strip() != remote:
            raise ValueError('Existing backup remote points elsewhere; resolve it before setup')
    else:
        run(['git', '-C', str(source), 'remote', 'add', 'backup', remote])
    CONFIG.parent.mkdir(parents=True, exist_ok=True)
    CONFIG.write_text(json.dumps(config, indent=2) + '\n')
    CONFIG.chmod(0o600)
    user_units = Path.home() / '.config/systemd/user'
    user_units.mkdir(parents=True, exist_ok=True)
    script = str(Path(__file__).resolve().with_name('lock_backup_watch.py')).replace('\\', '\\\\').replace('"', '\\"').replace('%', '%%')
    (user_units / SERVICE).write_text('[Unit]\nDescription=Private school snapshots on screen lock\nPartOf=graphical-session.target\nAfter=graphical-session.target\n\n[Service]\nExecStart=/usr/bin/python3 "' + script + '"\nRestart=on-failure\nRestartSec=5\n')
    autostart = Path.home() / '.config/autostart'
    autostart.mkdir(parents=True, exist_ok=True)
    (autostart / '42-lock-backup.desktop').write_text('[Desktop Entry]\nType=Application\nName=Private school lock backup\nExec=systemctl --user start 42-lock-backup.service\nX-GNOME-Autostart-enabled=true\n')
    subprocess.run(['systemctl', '--user', 'daemon-reload'], check=True)
    subprocess.run(['systemctl', '--user', 'restart', SERVICE], check=True)
    print('Private backup configured: ' + remote)
    print('Every lock saves school files. Codex, tmux and the desktop keep running.')
    print('Run privatebackupnow to test; privatebackupstatus shows setup status.')
    print('Set privatebackup=FALSE in ~/.42-shell-settings.zsh to disable lock backups and dli reminders.')

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--status', action='store_true')
    parser.add_argument('--source', default=os.environ.get('MAIN_REPO_ROOT'))
    parser.add_argument('--remote')
    parser.add_argument('--confirmed-private', action='store_true')
    parser.add_argument('--chat-private')
    args = parser.parse_args()
    if args.status:
        return 0 if status() else 1
    source = args.source or input('School repository path: ').strip()
    remote = args.remote or input('Create an EMPTY PRIVATE GitHub repo, then enter its SSH URL\n(e.g. git@github.com:YOUR_USER/42-school-wip.git): ').strip()
    install(source, remote, args.confirmed_private, args.chat_private)
    return 0

if __name__ == '__main__':
    try:
        raise SystemExit(main())
    except (OSError, ValueError, subprocess.SubprocessError) as error:
        print('Private backup setup failed: ' + str(error), file=sys.stderr)
        raise SystemExit(1)
