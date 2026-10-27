#!/usr/bin/env python3
"""Shared Go Break implementation for standalone and full-shell installations."""
import argparse
from datetime import datetime
from pathlib import Path
import os
import json
import subprocess
import signal
import threading
import sys

from leaveschool import break_checkpoint, git, wip_body, repositories
from git_side_push import changed_groups, validate_paths, side_push, stage_project


def checkpoint(repo, project_scope=False, codex_context=True, kind='WIP'):
    if kind not in ('WIP', 'EOD'):
        raise ValueError('Checkpoint kind must be WIP or EOD.')
    title = kind + ': checkpoint ' + datetime.now().astimezone().isoformat(timespec='seconds')
    repo = Path(git(repo, 'rev-parse', '--show-toplevel', capture=True).stdout.strip())
    git(repo, 'symbolic-ref', '--quiet', 'HEAD', capture=True)
    upstream = git(repo, 'rev-parse', '--symbolic-full-name', '@{upstream}', capture=True).stdout.strip()
    if not upstream.startswith('refs/remotes/'):
        raise ValueError('Configure a remote tracking branch first.')
    remote, branch = upstream.removeprefix('refs/remotes/').split('/', 1)
    for marker in ('MERGE_HEAD', 'CHERRY_PICK_HEAD', 'REVERT_HEAD', 'rebase-merge', 'rebase-apply', 'BISECT_LOG'):
        path = Path(git(repo, 'rev-parse', '--git-path', marker, capture=True).stdout.strip())
        if (path if path.is_absolute() else repo / path).exists():
            raise ValueError(f'Finish the current Git operation first ({marker}).')
    if git(repo, 'ls-files', '--unmerged', capture=True).stdout:
        raise ValueError('Resolve Git conflicts first.')
    validate_paths([path for group in changed_groups(repo) for path in group])
    if project_scope:
        projects = os.environ.get('PROJECTS_ROOT', 'Core Curriculum/Projects')
        body_builder = (lambda selected_repo, paths: wip_body(selected_repo, paths=paths)) if codex_context else None
        side_push(repo, projects, message=title + ' (debug/non-project)', body_builder=body_builder)
        stage_project(repo, projects)
    else:
        git(repo, 'add', '--all')
    if git(repo, 'diff', '--cached', '--name-only', capture=True).stdout:
        validate_paths(git(repo, 'diff', '--cached', '--name-only', '-z', capture=True).stdout.rstrip('\0').split('\0'))
        body = wip_body(repo) if codex_context else ''
        print('Using a Codex commit body.' if body else 'Using the existing fallback title only.', flush=True)
        git(repo, 'commit', '-m', title, *(['-m', body] if body else []))
    git(repo, 'push', remote, f'HEAD:refs/heads/{branch}')


def configured_paths(name, defaults=()):
    value = os.environ.get(name)
    paths = [*defaults, *(value.split(os.pathsep) if value is not None else [])]
    return [Path(path).expanduser().resolve() for path in paths if path]


def select_repositories(config, full_shell=False):
    """One repo by default; optional scan, includes and folder exclusions."""
    home = Path.home()
    root_value = os.environ.get('GOBREAK_REPOS_ROOT', config.get('scan_root', ''))
    if full_shell and 'GOBREAK_REPOS_ROOT' not in os.environ and not root_value:
        root_value = os.environ.get('DOCUMENTS_REPOS_ROOT', str(home / 'Documents'))
    root = Path(root_value).expanduser().resolve() if root_value else None
    mailbox = Path(os.environ.get('MAC_REMOTE_ROOT', home / 'Documents/mac-remote')).expanduser().resolve()
    excluded = set(configured_paths('GOBREAK_EXCLUDE_REPOS', config.get('exclude_repos', [])))
    excluded.update(configured_paths('DOCUMENTS_PUSH_EXCLUDE'))
    candidates = []
    if root:
        if not root.is_dir():
            raise ValueError(f'Missing repository search folder: {root}')
        candidates.extend(repositories(root, mailbox, excluded=excluded))
    if config.get('repo'):
        candidates.append(Path(config['repo']).expanduser().resolve())
    candidates.extend(configured_paths('GOBREAK_INCLUDE_REPOS', config.get('include_repos', [])))
    private = Path(os.environ.get('CODEX_PRIVATE_REPO_ROOT', home / 'Documents/42_Singapore_SUTD_hnah_private')).expanduser().resolve()
    if full_shell and private.is_dir():
        candidates.append(private)
    selected = set()
    for candidate in candidates:
        candidate = candidate.resolve()
        if any(candidate == folder or folder in candidate.parents for folder in excluded):
            continue
        # Explicit includes must be actual repository roots, not arbitrary folders.
        repo = Path(git(candidate, 'rev-parse', '--show-toplevel', capture=True).stdout.strip()).resolve()
        if repo != candidate:
            raise ValueError(f'Use the repository root instead of {candidate}: {repo}')
        selected.add(repo)
    return sorted(selected, key=lambda p: (-len(p.parts), str(p))), root or home / 'Documents', private


def print_greeting():
    print('\nGo Break — Git push + screen lock', flush=True)
    print('By hnah (Christopher Nah), 42SG Cohort 4', flush=True)
    print('Public repo: https://github.com/CrispyNuggetD/42_Singapore_SUTD', flush=True)
    print('Questions or feedback? Contact hnah on 42SG Slack.\n', flush=True)


def _exit_signal(number, frame):
    raise SystemExit(128 + number)


def main():
    failed = 0
    original_codex = os.environ.get('GOBREAK_CODEX_CONTEXT')
    previous_signals = {}
    try:
        if threading.current_thread() is threading.main_thread():
            for number in (signal.SIGTERM, signal.SIGHUP):
                previous_signals[number] = signal.getsignal(number)
                signal.signal(number, _exit_signal)
        parser = argparse.ArgumentParser(description=__doc__)
        scope = parser.add_mutually_exclusive_group(required=True)
        scope.add_argument('--repo', type=Path, help='Checkpoint only this repository')
        scope.add_argument('--all-repos', action='store_true', help='Use the full shell repository scan and private chat backup')
        scope.add_argument('--config', type=Path, help='Installed standalone selection settings')
        args = parser.parse_args()
        print_greeting()
        config = json.loads(args.config.read_text()) if args.config else {}
        preferences = config
        if not args.config:
            data = Path(os.environ.get('XDG_DATA_HOME', Path.home() / '.local/share'))
            saved = data / '42-gobreak/installation.json'
            if saved.exists():
                preferences = json.loads(saved.read_text())
        if not preferences.get('codex_context', True):
            os.environ['GOBREAK_CODEX_CONTEXT'] = 'FALSE'
        if args.repo:
            config['repo'] = str(args.repo)
        full_shell = args.all_repos or config.get('mode') == 'shell'
        repos, root, private = select_repositories(config, full_shell)
        print('Go Break repositories: ' + (', '.join(map(str, repos)) or '(none selected)'), flush=True)
        if full_shell:
            home = Path.home()
            failed = break_checkpoint(
                root, private,
                Path(os.environ.get('MAIN_REPO_ROOT', home / 'Documents/42_Singapore_SUTD')).resolve(), repos=repos)
        else:
            for repo in repos:
                try:
                    checkpoint(repo)
                except (OSError, ValueError, subprocess.SubprocessError) as error:
                    print(f'Checkpoint needs attention ({repo}): {error}', file=sys.stderr, flush=True)
                    failed = 1
    except (OSError, ValueError, subprocess.SubprocessError) as error:
        print(f'Checkpoint needs attention: {error}', file=sys.stderr, flush=True)
        failed = 1
    finally:
        # Shell wrappers own locking even when Python cannot start. Standalone
        # invocations lock here, including parse/config errors and signals.
        try:
            if os.environ.get('_GOBREAK_LOCK_IN_SHELL') != '1':
                try:
                    subprocess.run(['ft_lock'], check=True)
                except (OSError, subprocess.SubprocessError) as error:
                    print(f'Screen lock needs attention: {error}', file=sys.stderr, flush=True)
                    failed = 1
        finally:
            if original_codex is None:
                os.environ.pop('GOBREAK_CODEX_CONTEXT', None)
            else:
                os.environ['GOBREAK_CODEX_CONTEXT'] = original_codex
            for number, handler in previous_signals.items():
                signal.signal(number, handler)
    return failed


if __name__ == '__main__':
    sys.exit(main())
