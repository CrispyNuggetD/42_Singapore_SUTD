#!/usr/bin/env python3
"""Commit debug output and non-project changes before the project sync."""
import argparse
import os
from pathlib import Path, PurePosixPath
import subprocess

DEBUG_PATH = 'Core Curriculum/Projects/2_push_swap/tests/debug'

def git(repo, *args):
    env = dict(os.environ, GIT_TERMINAL_PROMPT='0')
    env['GIT_SSH_COMMAND'] = env.get('GIT_SSH_COMMAND', 'ssh') + ' -o BatchMode=yes -o ConnectTimeout=10'
    return subprocess.run(['git', '--literal-pathspecs', '-C', str(repo), *args],
                          env=env, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

def inside(path, directory):
    return path == directory or path.startswith(directory.rstrip('/') + '/')

def changed_groups(repo):
    entries = git(repo, 'status', '--porcelain=v1', '-z', '--untracked-files=all').stdout.split(b'\0')
    groups = []
    cursor = 0
    while cursor < len(entries) and entries[cursor]:
        entry = entries[cursor]; cursor += 1
        paths = [os.fsdecode(entry[3:])]
        if b'R' in entry[:2] or b'C' in entry[:2]:
            paths.append(os.fsdecode(entries[cursor])); cursor += 1
        groups.append(paths)
    return groups

def validate_paths(paths):
    for path in paths:
        p = PurePosixPath(path)
        name = p.name.lower()
        if p.is_absolute() or '..' in p.parts:
            raise ValueError('Invalid Git path')
        if name == '.env' or name.startswith('.env.') and not name.endswith(('example', 'sample')) or name in {'auth.json', 'credentials.json', '.git-credentials', 'id_rsa', 'id_ed25519'} or name.endswith(('.key', '.p12', '.pfx')):
            raise ValueError('Credential-like file found; side-push stopped for local review')

def side_push(repo, project_root='Core Curriculum/Projects', message='Update debug output and non-project files', push=True):
    repo = Path(repo).resolve(strict=True)
    project_root = str(PurePosixPath(project_root)).rstrip('/')
    if PurePosixPath(project_root).is_absolute() or '..' in PurePosixPath(project_root).parts or project_root == '.':
        raise ValueError('PROJECTS_ROOT must be a nonempty relative project directory')
    if push:
        tracking = git(repo, 'rev-parse', '--symbolic-full-name', '@{upstream}').stdout.decode().strip()
        if not tracking.startswith('refs/remotes/'):
            raise ValueError('Configure an upstream before side-push')
        remote, branch = tracking.removeprefix('refs/remotes/').split('/', 1)
    # Check errors before generating plots or changing the index.
    git(repo, 'symbolic-ref', '--quiet', 'HEAD')
    if git(repo, 'ls-files', '--unmerged').stdout:
        raise ValueError('Resolve Git conflicts before side-push')
    for marker in ('MERGE_HEAD', 'CHERRY_PICK_HEAD', 'REVERT_HEAD', 'rebase-merge', 'rebase-apply', 'BISECT_LOG'):
        location = Path(git(repo, 'rev-parse', '--git-path', marker).stdout.decode().strip())
        if not location.is_absolute():
            location = repo / location
        if location.exists():
            raise ValueError('Finish the current Git operation before side-push')
    groups = changed_groups(repo)
    plotter = repo / DEBUG_PATH / 'plot_summary.py'
    if plotter.is_file() and any(inside(path, DEBUG_PATH) for paths in groups for path in paths):
        subprocess.run(['python3', str(plotter)], cwd=repo, check=True)
        groups = changed_groups(repo)
    # A rename crossing into/out of project code stays in the main commit.
    paths = sorted({path for group in groups
                    if all(inside(path, DEBUG_PATH) or not inside(path, project_root) for path in group)
                    for path in group})
    validate_paths(paths)
    if paths:
        git(repo, 'add', '--all', '--', *paths)
        if git(repo, 'diff', '--cached', '--name-only', '--', *paths).stdout:
            git(repo, 'commit', '--only', '-m', message, '--', *paths)
            print(f'Side commit saved: {len(paths)} debug/non-project paths', flush=True)
    else:
        print('Side commit: no debug/non-project changes', flush=True)
    if push:
        git(repo, 'push', remote, 'HEAD:refs/heads/' + branch)
        print('Side-push succeeded; project changes remain for the main sync', flush=True)
    return paths

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', default=os.environ.get('MAIN_REPO_ROOT'))
    parser.add_argument('--projects-root', default=os.environ.get('PROJECTS_ROOT', 'Core Curriculum/Projects'))
    parser.add_argument('message', nargs='*')
    args = parser.parse_args()
    try:
        side_push(args.repo or '.', args.projects_root, ' '.join(args.message) or 'Update debug output and non-project files')
    except (OSError, ValueError, subprocess.SubprocessError) as error:
        print('gitpushswap failed: ' + str(error), flush=True)
        if isinstance(error, subprocess.CalledProcessError) and error.stderr:
            print(error.stderr.decode(errors='replace'), flush=True)
        raise SystemExit(1)
