#!/usr/bin/env python3
"""Play the bundled ASCII frames with optional VLC audio; no extra packages."""
import argparse
from pathlib import Path
import shutil
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parent


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--silent', action='store_true', help='Disable audio')
    parser.add_argument('--frames', type=int, help='Play only this many frames')
    args = parser.parse_args()
    frames = sorted((ROOT / 'frames-ascii').glob('*.txt'))
    if args.frames is not None:
        if args.frames < 1:
            parser.error('--frames must be positive')
        frames = frames[:args.frames]
    if not frames:
        parser.error('No ASCII frames found')
    if not sys.stdout.isatty():
        parser.error('Run this player in a terminal')
    audio = None
    try:
        if not args.silent:
            vlc = shutil.which('cvlc')
            if vlc:
                audio = subprocess.Popen(
                    [vlc, '--no-video', '--play-and-exit', str(ROOT / 'bad_apple.mp4')],
                    stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL,
                    stderr=subprocess.DEVNULL,
                )
            else:
                print('VLC not found; playing silently.', file=sys.stderr)
        sys.stdout.write('\033[?1049h\033[?25l\033[2J')
        start = time.monotonic()
        index = 0
        while index < len(frames):
            columns, rows = shutil.get_terminal_size()
            lines = frames[index].read_text().splitlines()
            # Sample the image to fit smaller panes without terminal wrapping.
            height = min(len(lines), max(1, rows - 1))
            width = min(max(map(len, lines)), max(1, columns - 1))
            rendered = []
            for y in range(height):
                line = lines[y * len(lines) // height]
                rendered.append(''.join(line[x * len(line) // width] for x in range(width)) if line else '')
            sys.stdout.write('\033[H' + '\033[K\n'.join(rendered) + '\033[K\033[J')
            sys.stdout.flush()
            index += 1
            time.sleep(max(0, start + index / 30 - time.monotonic()))
            index = max(index, int((time.monotonic() - start) * 30))
    except KeyboardInterrupt:
        pass
    finally:
        sys.stdout.write('\033[?25h\033[?1049l')
        sys.stdout.flush()
        if audio is not None and audio.poll() is None:
            audio.terminate()
            try:
                audio.wait(timeout=3)
            except subprocess.TimeoutExpired:
                audio.kill()
                audio.wait()


if __name__ == '__main__':
    main()
