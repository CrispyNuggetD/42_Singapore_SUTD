#!/usr/bin/env python3
"""Install/update the shared zshrc block and local helpers, preserving settings."""
import argparse
import ast
from datetime import datetime
import os
from pathlib import Path
import shutil
import shlex
import subprocess
import tempfile

BEGIN = '# >>> 42 shared zshrc >>>'
END = '# <<< 42 shared zshrc <<<'
RUNTIME_FILES = ('sync_zshrc.py', 'setup_zshrc.py', 'daily_terminal.py', 'Useful .zshrc edits (addition)')


def install_runtime(source_dir, runtime, check=False):
    """Install all helper dependencies locally, including the updater itself."""
    payloads = {}
    for name in RUNTIME_FILES:
        payload = (source_dir / name).read_bytes()
        if name.endswith('.py'):
            ast.parse(payload, filename=name)
        payloads[name] = payload
    if check:
        return
    runtime.mkdir(parents=True, exist_ok=True)
    for name, payload in payloads.items():
        destination = runtime / name
        if destination.exists() and destination.read_bytes() == payload:
            continue
        temporary = None
        try:
            with tempfile.NamedTemporaryFile(dir=runtime, delete=False) as output:
                temporary = Path(output.name)
                output.write(payload)
            os.replace(temporary, destination)
        finally:
            if temporary:
                temporary.unlink(missing_ok=True)


def bounds(text):
    lines = text.splitlines(keepends=True)
    starts, ends = [], []
    offset = 0
    for line in lines:
        if line.rstrip('\r\n') == BEGIN:
            starts.append(offset)
        if line.rstrip('\r\n') == END:
            ends.append(offset + len(line))
        offset += len(line)
    if not starts and not ends:
        return None
    if len(starts) != 1 or len(ends) != 1 or starts[0] >= ends[0]:
        raise ValueError('Missing, duplicate, or reversed shared markers; no changes made.')
    return starts[0], ends[0]


def read(path):
    # Preserve even CRLF/local content byte-for-byte outside the shared block.
    return path.read_bytes().decode('utf-8')


def sync(source, target, check=False, runtime=None):
    source = source.expanduser().resolve(strict=True)
    runtime = (runtime or Path.home() / '.local/share/42-shell').expanduser().resolve()
    target = target.expanduser().resolve()
    if source == target:
        raise ValueError('Source and target must be different files.')
    shared = read(source)
    extent = bounds(shared)
    if extent is None:
        raise ValueError('Source has no shared markers; no changes made.')
    block = shared[extent[0]:extent[1]]
    # Bootstrap setup from wherever this clone was installed. A personal profile
    # takes precedence, and MAIN_REPO_ROOT may point to a different repository.
    block = block.replace(
        'export ZSHRC_SHARED_ROOT="${ZSHRC_SHARED_ROOT:-$HOME/Documents/42_Singapore_SUTD}"',
        'export ZSHRC_SHARED_ROOT=${ZSHRC_SHARED_ROOT:-' + shlex.quote(str(source.parent)) + '}',
    )
    block = block.replace(
        'export ZSHRC_RUNTIME_ROOT="${ZSHRC_RUNTIME_ROOT:-$HOME/.local/share/42-shell}"',
        'export ZSHRC_RUNTIME_ROOT=${ZSHRC_RUNTIME_ROOT:-' + shlex.quote(str(runtime)) + '}',
    )
    if not block.endswith('\n'):
        block += '\n'
    existed = target.exists()
    original = read(target) if existed else ''
    extent = bounds(original)
    if extent is None:
        # Unknown legacy files are never guessed at or deleted. Append the shared
        # definitions so they take precedence over older copies above them.
        updated = original + ('\n' if original and not original.endswith('\n') else '') + block
    else:
        updated = original[:extent[0]] + block + original[extent[1]:]
    if updated == original:
        install_runtime(source.parent, runtime, check)
        print('Shared zshrc helpers are already up to date.')
        return
    target.parent.mkdir(parents=True, exist_ok=True)
    temporary = None
    try:
        with tempfile.NamedTemporaryFile(dir=target.parent, prefix='.zshrc-sync-', delete=False) as candidate:
            temporary = Path(candidate.name)
            candidate.write(updated.encode('utf-8'))
        subprocess.run(['zsh', '-n', str(temporary)], check=True)
        install_runtime(source.parent, runtime, check)
        if check:
            print('Shared zshrc update available; syntax check passed. No changes made.')
            return
        if target.exists() != existed or (existed and read(target) != original):
            raise ValueError('Local zshrc changed during sync; rerun to avoid losing edits.')
        if existed:
            backup = target.with_name(target.name + '.bak-' + datetime.now().strftime('%Y%m%d-%H%M%S-%f'))
            shutil.copy2(target, backup)
            shutil.copymode(target, temporary)
            print(f'Backup: {backup}')
        # target is resolved first, so ~/.zshrc symlinks remain intact.
        os.replace(temporary, target)
        print(f'Updated shared helpers in {target}')
        if extent is None:
            print('First install: existing content preserved above the shared block.')
        print('Reload your .zshrc or open a new terminal to use the update.')
    finally:
        if temporary is not None:
            temporary.unlink(missing_ok=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=Path(__file__).resolve().with_name('Useful .zshrc edits (addition)'))
    parser.add_argument('--target', type=Path, default=Path(os.environ.get('ZDOTDIR') or Path.home()) / '.zshrc')
    parser.add_argument('--check', action='store_true', help='Check for an update and validate syntax without replacing the target')
    parser.add_argument('--runtime', type=Path, default=Path(os.environ.get('ZSHRC_RUNTIME_ROOT', str(Path.home() / '.local/share/42-shell'))))
    args = parser.parse_args()
    try:
        sync(args.source, args.target, args.check, args.runtime)
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        parser.exit(1, f'zshrc sync failed: {error}\n')


if __name__ == '__main__':
    main()
