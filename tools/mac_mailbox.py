#!/usr/bin/env python3
"""Display the Mac processor in tmux without replacing its identity or worker code."""
import argparse
import fcntl
import os
from pathlib import Path
import subprocess
import sys


def running(runtime):
    path = runtime / '.mailbox/mac/worker.lock'
    if not path.exists():
        return False
    with path.open('r') as lock:
        try:
            fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except BlockingIOError:
            return True
        return False


def command(runtime, source, mode):
    if mode not in ('service', 'tmux'):
        raise ValueError('MAC_MAILBOX_MODE must be service or tmux.')
    active = running(runtime)
    if mode == 'tmux' and not active:
        entry = runtime / 'mac.sh'
        if not entry.is_file():
            raise ValueError('Installed Mac processor is missing; use its normal local installer first.')
        print('Running the Mac processor in tmux; closing this pane stops the listener.', flush=True)
        return ['bash', str(entry), 'run', '--full-access']
    if not active:
        entry = source / 'mac.sh'
        if not entry.is_file():
            raise ValueError('Set MAC_REMOTE_ROOT to this Mac’s mailbox source clone.')
        subprocess.run(['bash', str(entry), 'start'], check=True)
    elif mode == 'tmux':
        print('Existing processor kept running; showing its log. Switch ownership locally only when idle.', flush=True)
    print('Mac mailbox service log. Detaching or closing this pane keeps the listener running.', flush=True)
    logs = runtime / '.mailbox/mac'
    return ['tail', '-n', '120', '-F', str(logs / 'daemon.log'), str(logs / 'daemon-error.log')]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, required=True)
    args = parser.parse_args()
    if sys.platform != 'darwin':
        parser.error('This helper runs on the Mac processor only.')
    runtime = Path(os.environ.get('MAC_MAILBOX_RUNTIME', str(Path.home() / 'Library/Application Support/mac-remote')))
    try:
        invocation = command(runtime, args.source, os.environ.get('MAC_MAILBOX_MODE', 'service'))
        os.execvp(invocation[0], invocation)
    except (OSError, ValueError, subprocess.SubprocessError) as error:
        parser.exit(1, 'Mac mailbox needs local attention: ' + str(error) + '\n')


if __name__ == '__main__':
    main()
