#!/usr/bin/env python3
"""Install the Bad Apple terminal player in ~/joke without sudo."""
from pathlib import Path
import shutil
import subprocess
import tempfile


def install(destination):
    destination.mkdir(parents=True, exist_ok=True)
    frames = destination / 'frames-ascii'
    video = destination / 'bad_apple.mp4'
    if not frames.is_dir() or not video.is_file():
        print('Downloading Bad Apple frames and audio…', flush=True)
        with tempfile.TemporaryDirectory(prefix='badapple-') as temporary:
            checkout = Path(temporary) / 'repo'
            subprocess.run([
                'git', 'clone', '--depth', '1',
                'https://github.com/trung-kieen/bad-apple-ascii.git', str(checkout),
            ], check=True)
            if not frames.exists():
                shutil.copytree(checkout / 'frames-ascii', frames)
            if not video.exists():
                shutil.copy2(checkout / 'bad_apple.mp4', video)
    if not frames.is_dir() or not any(frames.glob('*.txt')) or not video.is_file():
        raise ValueError(f'Expected video and ASCII frames in {destination}; check existing files.')
    shutil.copy2(Path(__file__).with_name('badapple_player.py'), destination / 'play.py')
    launcher = destination / 'play.sh'
    launcher.write_text(
        '#!/usr/bin/env bash\n'
        'exec python3 "$(cd -- "$(dirname -- "$0")" && pwd)/play.py" "$@"\n'
    )
    launcher.chmod(0o755)


if __name__ == '__main__':
    try:
        install(Path.home() / 'joke')
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        raise SystemExit(f'Bad Apple setup failed: {error}')
