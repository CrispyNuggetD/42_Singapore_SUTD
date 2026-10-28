#!/usr/bin/env python3
"""Daily official standalone CLI/daemon updates without stopping live work."""
import argparse
import fcntl
import os
from pathlib import Path
import subprocess
import tempfile
import time


def update(force=False):
    home = Path(os.environ.get('CODEX_HOME', str(Path.home() / '.codex')))
    state = home / 'daily-update'
    state.mkdir(parents=True, exist_ok=True)
    with (state / 'lock').open('a') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        stamp = state / 'success'
        if not force and stamp.exists() and time.time() - stamp.stat().st_mtime < 86400:
            print('Codex CLI/daemon packages: update already completed within 24 hours.')
            return
        env = dict(os.environ, CODEX_NON_INTERACTIVE='1', CODEX_RELEASE='latest')
        env['CODEX_INSTALL_DIR'] = os.environ.get('CODEX_INSTALL_DIR', str(Path.home() / '.local/bin'))
        # Never inherit installer-internal migration or scheduled-updater guards.
        for key in list(env):
            if key.startswith('CODEX_INSTALL_') and key != 'CODEX_INSTALL_DIR':
                del env[key]
        with tempfile.TemporaryDirectory(prefix='codex-update-') as directory:
            installer = str(Path(directory) / 'install.sh')
            subprocess.run(['curl', '--fail', '--silent', '--show-error', '--location',
                            '--connect-timeout', '10', '--max-time', '60',
                            'https://chatgpt.com/codex/install.sh', '-o', installer],
                           check=True, timeout=70)
            for daemon in (False, True):
                print('Updating Codex ' + ('daemon package' if daemon else 'CLI package'), flush=True)
                package_env = dict(env, CODEX_INSTALL_DAEMON_ONLY='1' if daemon else '0')
                subprocess.run(['sh', installer], env=package_env, check=True, timeout=600)
        stamp.touch()
        print('Codex CLI/daemon packages updated. Live sessions were not forcibly restarted; '
              'new CLI launches use the selected package.')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--force', action='store_true', help='Ignore the successful daily check')
    args = parser.parse_args()
    try:
        update(args.force)
    except (OSError, subprocess.SubprocessError) as error:
        print('Codex update failed (will retry): ' + str(error))
        return 1
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
