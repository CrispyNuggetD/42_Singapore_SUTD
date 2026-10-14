#!/usr/bin/env python3
"""Install or refresh the shared Go Break command and desktop icon."""
import argparse
import ast
import hashlib
import json
import os
from pathlib import Path
import shlex
import shutil
import subprocess

from gobreak import print_greeting

HELPERS = ('gobreak.py', 'leaveschool.py', 'git_side_push.py')
DESKTOP_ID = 'local.42-gobreak.desktop'


def pin_to_dock():
    """Append our app to GNOME favorites without replacing existing pins."""
    if not shutil.which('gsettings'):
        return False
    try:
        command = ['gsettings', 'get', 'org.gnome.shell', 'favorite-apps']
        value = subprocess.run(command, check=True, capture_output=True, text=True, timeout=10).stdout.strip()
        if value.startswith('@as '):
            value = value[4:]
        favorites = ast.literal_eval(value)
        if not isinstance(favorites, list) or not all(isinstance(app, str) for app in favorites):
            return False
        if DESKTOP_ID in favorites:
            return True
        writable = subprocess.run(['gsettings', 'writable', 'org.gnome.shell', 'favorite-apps'],
                                  check=True, capture_output=True, text=True, timeout=10)
        if writable.stdout.strip() != 'true':
            return False
        subprocess.run(['gsettings', 'set', 'org.gnome.shell', 'favorite-apps',
                        repr([*favorites, DESKTOP_ID])],
                       check=True, capture_output=True, text=True, timeout=10)
        return True
    except (OSError, ValueError, SyntaxError, subprocess.SubprocessError):
        return False


def desktop_quote(value):
    return '"' + str(value).replace('\\', '\\\\').replace('"', '\\"').replace('`', '\\`').replace('$', '\\$').replace('%', '%%') + '"'


def copy_changed(source, destination):
    if source.resolve() == destination.resolve():
        return
    payload = source.read_bytes()
    if not destination.exists() or destination.read_bytes() != payload:
        destination.write_bytes(payload)


def install(source, repo=None, shared_runtime=None, check=False, automatic=False,
            mode_override=None, scan_root=None, include_repos=None, exclude_repos=None,
            pin=None, codex_context=None):
    """Refresh both installations; automatic shell sync preserves icon scope."""
    print_greeting()
    source = Path(source).resolve()
    data = Path(os.environ.get('XDG_DATA_HOME', Path.home() / '.local/share'))
    runtime = data / '42-gobreak'
    apps = data / 'applications'
    config_path = runtime / 'installation.json'
    config = json.loads(config_path.read_text()) if config_path.exists() else {}
    if pin is not None:
        config['pin_to_dock'] = pin
    config.setdefault('pin_to_dock', True)
    if codex_context is not None:
        config['codex_context'] = codex_context
    config.setdefault('codex_context', True)
    config.setdefault('include_repos', [])
    config.setdefault('exclude_repos', [])
    if shared_runtime is not None:
        config['shared_runtime'] = str(Path(shared_runtime).resolve())
    if repo is not None:
        repo = Path(subprocess.check_output(['git', '-C', str(repo), 'rev-parse', '--show-toplevel'], text=True).strip())
        config.update(mode='repo', repo=str(repo))
    if mode_override:
        config['mode'] = mode_override
        if mode_override == 'shell':
            config.pop('repo', None)
    for name, value in (('scan_root', scan_root), ('include_repos', include_repos), ('exclude_repos', exclude_repos)):
        if value is not None:
            config[name] = [str(Path(p).expanduser().resolve()) for p in value] if isinstance(value, list) else str(Path(value).expanduser().resolve())
    if not config.get('mode'):
        config['mode'] = 'shell'
    mode = config['mode']
    if mode not in ('repo', 'shell'):
        raise ValueError('Unknown Go Break installation mode')
    icon = source / 'Shell automations/gobreak/gobreak.png'
    if not icon.exists():
        icon = source / 'gobreak.png'  # Installed shell runtime layout.
    for name in HELPERS:
        (source / name).read_bytes()
    icon.read_bytes()
    # A shell sync may be run without a graphical desktop. Still update the code.
    required = ['git', 'python3', 'bash', 'flock', 'gnome-terminal', 'ft_lock']
    if mode == 'shell':
        required.append('zsh')
    missing = [tool for tool in required if not shutil.which(tool)]
    if missing and not automatic:
        raise ValueError('Required campus desktop commands missing: ' + ', '.join(missing))
    if check:
        print(f'Go Break would be refreshed (icon scope: {mode}, pin to dock: {config["pin_to_dock"]}); no changes made.')
        return
    runtime.mkdir(parents=True, exist_ok=True)
    for name in HELPERS:
        copy_changed(source / name, runtime / name)
    copy_changed(icon, runtime / 'gobreak.png')
    # A standalone update also refreshes the main shell's copy, if enrolled.
    shared = Path(config['shared_runtime']) if config.get('shared_runtime') else None
    if shared and shared.is_dir():
        for name in (*HELPERS, 'install_gobreak.py'):
            copy_changed(source / name, shared / name)
        copy_changed(icon, shared / 'gobreak.png')
    config_path.write_text(json.dumps(config, indent=2) + '\n')
    if missing:
        print('Go Break code refreshed; desktop installation deferred until available: ' + ', '.join(missing))
        return
    if mode == 'repo':
        target = config['repo']
        identity = hashlib.sha256(target.encode()).hexdigest()[:16]
        launcher = runtime / ('launch-' + identity)
        selection = runtime / ('selection-' + identity + '.json')
        selection.write_text(json.dumps(config, indent=2) + '\n')
        command = [shutil.which('python3'), str(runtime / 'gobreak.py'), '--config', str(selection)]
    else:
        launcher = runtime / 'launch-shell'
        # Load the owner's settings from their current zshrc, then run shared code.
        command = [shutil.which('zsh'), '-i', '-c',
                   'GOBREAK_REPOS_ROOT="${GOBREAK_REPOS_ROOT-${DOCUMENTS_REPOS_ROOT:-$HOME/Documents}}" '
                   'GOBREAK_INCLUDE_REPOS="${GOBREAK_INCLUDE_REPOS:-}" '
                   'GOBREAK_EXCLUDE_REPOS="${GOBREAK_EXCLUDE_REPOS:-}" '
                   'command python3 "$1" --config "$2"',
                   'gobreak-shell', str(runtime / 'gobreak.py'), str(config_path)]
    launcher.write_text(
        '#!/bin/bash\n'
        'exec 9>"${XDG_RUNTIME_DIR:-/tmp}/42-gobreak-${UID}.lock"\n'
        'flock -n 9 || { echo "A Go Break checkpoint is already running."; exit 0; }\n'
        + shlex.join(command) + '\n'
        'result=$?\n'
        'if [ "$result" -ne 0 ]; then\n'
        '  read -r -p "Review the error after unlocking. Press Enter to close. " _\n'
        'fi\nexit "$result"\n')
    launcher.chmod(0o755)
    apps.mkdir(parents=True, exist_ok=True)
    (apps / DESKTOP_ID).write_text(
        '[Desktop Entry]\nVersion=1.0\nType=Application\nName=Go Break (Git push + lock)\n'
        'Comment=Save and push a WIP checkpoint, then lock\n'
        f'Exec={desktop_quote(shutil.which("gnome-terminal"))} --title="Go Break" -- {desktop_quote(launcher)}\n'
        f'Icon={runtime / "gobreak.png"}\nTerminal=false\nCategories=Utility;\nStartupNotify=true\n')
    if mode == 'repo':
        subprocess.run(['git', '-C', config['repo'], 'config', '--local', 'alias.gobreak',
                        '!' + shlex.quote(str(launcher))], check=True)
        print(f'Installed/refreshed Go Break for one repo: {config["repo"]}')
    else:
        print('Installed/refreshed Go Break for the main shell’s configured repository scan.')
    if config['pin_to_dock']:
        if pin_to_dock():
            print('Ubuntu icon installed and pinned to the dock.')
        else:
            print('Ubuntu icon installed; automatic dock pinning is unavailable in this session.')
            print('Open Show Applications, right-click “Go Break (Git push + lock)”, and choose “Add to Favorites”.')
    else:
        print('Ubuntu icon installed in Show Applications; dock pinning skipped (--no-pin).')
    if config['codex_context']:
        print('An already logged-in Codex CLI can automatically summarize the staged diff in the commit body.')
        if not shutil.which('codex'):
            print('Debug: Codex CLI not found; using the fallback title. Configure Codex, or rerun with --no-codex to disable this message and summaries.')
    else:
        print('Codex summaries disabled (--no-codex); using the existing WIP timestamp title only.')
    print('Codex is optional and is not installed or configured. --no-codex or GOBREAK_CODEX_CONTEXT=FALSE disables summaries.')
    print('Go Break installation does not edit .zshrc, commit, push, or lock.')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    scope = parser.add_mutually_exclusive_group()
    scope.add_argument('--repo', type=Path, help='Repository to checkpoint (default: current directory)')
    scope.add_argument('--shell', action='store_true', help='Use the main shell’s multiple-repository scope')
    parser.add_argument('--check', action='store_true')
    parser.add_argument('--no-pin', action='store_true', help='Skip automatic dock pinning; remembered by shell syncs')
    codex = parser.add_mutually_exclusive_group()
    codex.add_argument('--no-codex', dest='codex_context', action='store_false', help='Disable Codex summaries and missing-CLI messages; remembered across updates')
    codex.add_argument('--codex', dest='codex_context', action='store_true', help='Re-enable optional Codex summaries')
    parser.set_defaults(codex_context=None)
    parser.add_argument('--scan-root', type=Path, help='Also discover repositories below this folder')
    parser.add_argument('--include-repo', type=Path, action='append', help='Additional repo; repeat to include several')
    parser.add_argument('--exclude-repo', type=Path, action='append', help='Exclude this folder and everything below it; repeat as needed')
    args = parser.parse_args()
    try:
        source = Path(__file__).resolve().parent
        install(source, repo=None if args.shell else (args.repo or Path.cwd()), check=args.check,
                mode_override='shell' if args.shell else None, scan_root=args.scan_root,
                include_repos=args.include_repo, exclude_repos=args.exclude_repo,
                pin=not args.no_pin, codex_context=args.codex_context)
    except (OSError, ValueError, subprocess.SubprocessError) as error:
        parser.exit(1, f'Go Break installation failed: {error}\n')


if __name__ == '__main__':
    main()
