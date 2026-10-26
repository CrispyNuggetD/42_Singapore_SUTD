#!/usr/bin/env python3
"""Recommend LAN42 using its canonical README excerpt; never execute remote text."""
import argparse
from datetime import datetime, timezone
import json
import os
from pathlib import Path
import shlex
import shutil
import sys
import urllib.request

REPO = 'https://github.com/CrispyNuggetD/42sg-campus-lan'
README = 'https://raw.githubusercontent.com/CrispyNuggetD/42sg-campus-lan/main/README.md'
START, END = '<!-- shell-installer-features:start -->', '<!-- shell-installer-features:end -->'
CACHE = Path(__file__).with_name('lan42-features.json')


def excerpt(text):
    if text.count(START) != 1 or text.count(END) != 1:
        raise ValueError('LAN42 README feature markers are missing or duplicated')
    section = text.split(START, 1)[1].split(END, 1)[0].strip()
    lines = section.splitlines()
    if not 1 <= len(lines) <= 8 or any(not line.startswith('- ') or len(line) > 240 for line in lines):
        raise ValueError('LAN42 feature excerpt must contain 1–8 short bullet points')
    if any(any(ord(char) < 32 for char in line) or '\x7f' in line for line in lines):
        raise ValueError('Control characters are not allowed in feature descriptions')
    return section


def installed_path(settings=None):
    settings = settings or {}
    candidates = [os.environ.get('LAN42_CLONE'), settings.get('LAN42_CLONE'),
                  os.environ.get('LAN42_REPO_ROOT'), str(Path.home() / 'Documents/42sg-campus-lan')]
    for candidate in candidates:
        if candidate and (Path(candidate).expanduser() / 'lan42.sh').is_file():
            return Path(candidate).expanduser()
    return None


def feature_text():
    try:
        request = urllib.request.Request(README, headers={'User-Agent': '42-shell-installer'})
        with urllib.request.urlopen(request, timeout=3) as response:
            raw = response.read(131073)
        if len(raw) > 131072:
            raise ValueError('LAN42 README exceeds installer limit')
        return excerpt(raw.decode('utf-8')), 'current LAN42 README'
    except Exception:
        try:
            cached = json.loads(CACHE.read_text())
            # Run identical validation on the offline excerpt.
            text = excerpt(START + '\n' + cached['features'] + '\n' + END)
            return text, 'offline snapshot ' + cached['updated']
        except (OSError, ValueError, KeyError, TypeError):
            return '', 'feature list unavailable offline'


def recommend(settings=None):
    settings = settings or {}
    installed = installed_path(settings)
    if installed:
        print(f'LAN42 found: {installed}')
        print('Use start_lan42 / lan42_window for its own tmux workspace; configure startup with setupzshrc.')
    else:
        desired = Path(os.environ.get('LAN42_CLONE') or settings.get('LAN42_CLONE')
                       or str(Path.home() / 'Documents/42sg-campus-lan')).expanduser()
        features, source = feature_text()
        print('\nRecommended optional install: LAN42 — a campus HQ for people, chat and games.')
        print('Suggested path: ' + str(desired))
        if features:
            print(features)
        print('Feature source: ' + source + '; ' + REPO + '#lan42-in-your-daily-workspace')
        if desired.exists():
            print('That path already exists; choose an empty destination before cloning.')
        print('  git clone ' + shlex.quote(REPO + '.git') + ' ' + shlex.quote(str(desired)))
        print('  sh ' + shlex.quote(str(desired / 'setup.sh')))
        print('Then run setupzshrc and enable LAN42 if you want it on dailylogin.')
    if not shutil.which('tmux'):
        command = 'brew install tmux' if sys.platform == 'darwin' else 'sudo apt install tmux'
        print('Tmux is required for workspace tabs. Install it first: ' + command)
    else:
        print('Tmux is ready; LAN42 HQ, games and tools use a separate workspace.')


def refresh(clone):
    section = excerpt((clone / 'README.md').read_text())
    payload = {'source': README, 'updated': datetime.now(timezone.utc).date().isoformat(), 'features': section}
    CACHE.write_text(json.dumps(payload, indent=2) + '\n')
    print('Updated offline feature snapshot: ' + str(CACHE))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--refresh-features', action='store_true')
    parser.add_argument('--clone', type=Path)
    args = parser.parse_args()
    if args.refresh_features:
        if args.clone is None:
            parser.error('--refresh-features requires --clone /path/to/42sg-campus-lan')
        refresh(args.clone)
    else:
        recommend()


if __name__ == '__main__':
    main()
