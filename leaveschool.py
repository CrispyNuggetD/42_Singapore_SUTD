#!/usr/bin/env python3
"""End-of-day repository sync and private Codex session backup."""
import argparse
from datetime import datetime
import os
from pathlib import Path
import shutil
import subprocess
import tempfile


def git(repo, *args, capture=False):
    env = dict(os.environ, GIT_TERMINAL_PROMPT='0')
    env['GIT_SSH_COMMAND'] = env.get('GIT_SSH_COMMAND', 'ssh') + ' -o BatchMode=yes -o ConnectTimeout=10'
    return subprocess.run(['git', '-C', str(repo), *args], check=True,
                          env=env, text=True, stdout=subprocess.PIPE if capture else None)


def repositories(root, mailbox):
    found = []
    def scan_error(error):
        raise error
    for current, dirs, files in os.walk(root, onerror=scan_error, followlinks=False):
        path = Path(current)
        if path.resolve() == mailbox:
            dirs[:] = []
            continue
        if '.git' in dirs or '.git' in files:
            found.append(path)
        dirs[:] = [d for d in dirs if d != '.git' and not (path / d).is_symlink()]
    # Commit nested repositories before their parents (including submodules).
    return sorted(found, key=lambda p: len(p.parts), reverse=True)


def push(repo):
    print(f'\nSaving and pushing: {repo}', flush=True)
    git(repo, 'symbolic-ref', '--quiet', 'HEAD', capture=True)
    remote = git(repo, 'rev-parse', '--symbolic-full-name', '@{upstream}', capture=True).stdout.strip()
    if not remote.startswith('refs/remotes/'):
        raise ValueError(f'{repo}: configure a remote tracking branch first')
    for marker in ('MERGE_HEAD', 'CHERRY_PICK_HEAD', 'REVERT_HEAD', 'rebase-merge', 'rebase-apply', 'BISECT_LOG'):
        marker_path = Path(git(repo, 'rev-parse', '--git-path', marker, capture=True).stdout.strip())
        if not marker_path.is_absolute():
            marker_path = repo / marker_path
        if marker_path.exists():
            raise ValueError(f'{repo}: unfinished Git operation ({marker})')
    if git(repo, 'ls-files', '--unmerged', capture=True).stdout:
        raise ValueError(f'{repo}: unresolved conflicts')
    git(repo, 'add', '--all')
    if git(repo, 'diff', '--cached', '--name-only', capture=True).stdout:
        git(repo, 'commit', '-m', 'leaveschool: save work ' + datetime.now().astimezone().isoformat(timespec='seconds'))
    # Explicit upstream push: no force, merges, or automatic branch creation.
    tracking = remote.removeprefix('refs/remotes/')
    remote_name, branch = tracking.split('/', 1)
    git(repo, 'push', remote_name, f'HEAD:refs/heads/{branch}')


def backup(source, private, main):
    private = private.resolve(strict=True)
    top = Path(git(private, 'rev-parse', '--show-toplevel', capture=True).stdout.strip()).resolve()
    url = git(private, 'remote', 'get-url', 'origin', capture=True).stdout.strip()
    if top != private or not url.removesuffix('.git').endswith('/42_Singapore_SUTD_hnah_private'):
        raise ValueError('Chat backup requires the separate 42_Singapore_SUTD_hnah_private clone')
    target = private / '42/codex_cache'
    resolved = target.resolve()
    if resolved != target or main.resolve() == private or main.resolve() in resolved.parents:
        raise ValueError('Chat backup must remain inside the separate private repo; no symlink destinations')
    if not source.is_dir():
        raise ValueError(f'Missing Codex sessions directory: {source}')
    copied = 0
    for src in source.rglob('*.jsonl'):
        if src.is_symlink() or not src.is_file() or source.resolve() not in src.resolve().parents:
            continue
        dst = target / 'sessions' / src.relative_to(source)
        if dst.resolve() != dst:
            raise ValueError(f'Refusing symlink destination: {dst}')
        before = src.stat()
        if dst.exists() and dst.stat().st_size == before.st_size and dst.stat().st_mtime_ns == before.st_mtime_ns:
            continue
        dst.parent.mkdir(parents=True, exist_ok=True)
        fd, temporary = tempfile.mkstemp(prefix='.chat-', dir=dst.parent)
        os.close(fd)
        try:
            shutil.copyfile(src, temporary)
            os.utime(temporary, ns=(before.st_atime_ns, before.st_mtime_ns))
            os.replace(temporary, dst)
        finally:
            Path(temporary).unlink(missing_ok=True)
        copied += 1
    print(f'Copied {copied} new/updated chats to {target}', flush=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('action', choices=('push-all', 'backup', 'push-private'))
    args = parser.parse_args()
    home = Path.home()
    root = Path(os.environ.get('DOCUMENTS_REPOS_ROOT', home / 'Documents')).resolve()
    private = Path(os.environ.get('CODEX_PRIVATE_REPO_ROOT', home / 'Documents/42_Singapore_SUTD_hnah_private')).resolve()
    main_repo = Path(os.environ.get('MAIN_REPO_ROOT', home / 'Documents/42_Singapore_SUTD'))
    try:
        if args.action == 'backup':
            backup(Path(os.environ.get('CODEX_HOME', home / '.codex')) / 'sessions', private, main_repo)
            return 0
        if args.action == 'push-private':
            push(private)
            return 0
        if not root.is_dir():
            raise ValueError(f'Missing repository search folder: {root}')
        mailbox = Path(os.environ.get('MAC_REMOTE_ROOT', home / 'Documents/mac-remote')).resolve()
        repos = repositories(root, mailbox)
        failed = 0
        for repo in repos:
            try:
                push(repo)
            except (OSError, ValueError, subprocess.CalledProcessError) as error:
                print(f'Needs attention: {error}', flush=True)
                failed += 1
        print(f'Repository sync: {len(repos) - failed}/{len(repos)} succeeded; {failed} need attention.')
        return int(bool(failed))
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        print(f'leaveschool: {error}', flush=True)
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
