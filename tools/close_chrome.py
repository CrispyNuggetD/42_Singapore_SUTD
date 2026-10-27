#!/usr/bin/env python3
"""Ask the current user's Chrome browsers to exit cleanly; never force kill."""
import argparse
import os
from pathlib import Path
import signal
import re
import subprocess
import sys
import time


def identity(directory):
    # Linux start time prevents signaling/waiting on a reused process ID.
    fields = (directory / 'stat').read_text().rpartition(')')[2].split()
    return None if fields[0] == 'Z' else fields[19]


def browsers(proc=Path('/proc')):
    found = {}
    for directory in proc.iterdir():
        if not directory.name.isdigit():
            continue
        try:
            if directory.stat().st_uid != os.getuid():
                continue
            if (directory / 'exe').resolve().name not in ('chrome', 'chromium', 'chromium-browser'):
                continue
            # Chrome can rewrite argv into a single space-separated process title.
            args = (directory / 'cmdline').read_bytes().replace(b'\0', b' ')
            if re.search(rb'(?:^|\s)--type(?:=|\s|$)', args):
                continue
            token = identity(directory)
            if token:
                found[int(directory.name)] = token
        except (OSError, IndexError, ValueError):
            continue
    return found


def still_running(pid, token, proc=Path('/proc')):
    try:
        return identity(proc / str(pid)) == token
    except (OSError, IndexError):
        return False


def close(timeout=30):
    if sys.platform == 'darwin':
        # The running guard avoids launching Chrome just to close it.
        subprocess.run(['osascript', '-e',
                        'if application "Google Chrome" is running then\n'
                        'tell application "Google Chrome" to quit\n'
                        'repeat while application "Google Chrome" is running\n'
                        'delay 0.2\nend repeat\nend if'],
                       check=True, timeout=timeout)
        return
    if not sys.platform.startswith('linux'):
        raise RuntimeError('Automatic Chrome quit is supported on Linux and macOS.')
    running = browsers()
    if not running:
        print('Chrome is already closed.')
        return
    print('Asking Chrome to quit cleanly...', flush=True)
    for pid, token in running.items():
        if still_running(pid, token):
            try:
                # Chromium handles SIGHUP via its normal graceful Exit() path.
                # Signal only browser parents, never renderers or process groups.
                os.kill(pid, signal.SIGHUP)
            except ProcessLookupError:
                pass
    deadline = time.monotonic() + timeout
    while any(still_running(pid, token) for pid, token in running.items()):
        if time.monotonic() >= deadline:
            raise RuntimeError('Chrome is still running; finish any close prompts locally and retry leavesch. It was not force-killed.')
        time.sleep(0.2)
    print('Chrome exited cleanly.')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true', help='Report Linux browser process count without closing anything')
    args = parser.parse_args()
    try:
        if args.check:
            print(f'Chrome browser processes: {len(browsers())}')
        else:
            close()
    except (OSError, RuntimeError, subprocess.SubprocessError) as error:
        print(f'Chrome quit needs attention: {error}', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
