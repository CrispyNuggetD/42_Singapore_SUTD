#!/usr/bin/env python3
"""Bounded dailylogin Git recovery. Codex selects an action, never edits the repo."""
import argparse
import fcntl
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import signal
import stat
import subprocess
import tempfile
import time

ACTIONS = ('retry', 'merge_upstream', 'stale_index_lock', 'needs_user')


def git(repo, *args, check=True):
    env = dict(os.environ, GIT_TERMINAL_PROMPT='0', GIT_OPTIONAL_LOCKS='0',
               GIT_SSH_COMMAND='ssh -o BatchMode=yes -o ConnectTimeout=10')
    return subprocess.run(['git', '-C', str(repo), '-c', 'merge.autostash=false',
                           '-c', 'rebase.autostash=false', *args], env=env,
                          text=True, capture_output=True, timeout=45, check=check)


def snapshot(repo):
    head = git(repo, 'rev-parse', 'HEAD').stdout.strip()
    upstream = git(repo, 'rev-parse', '@{upstream}').stdout.strip()
    branch = git(repo, 'symbolic-ref', '--quiet', 'HEAD').stdout.strip()
    dirty = git(repo, 'status', '--porcelain=v1', '--untracked-files=all').stdout
    directory = Path(git(repo, 'rev-parse', '--absolute-git-dir').stdout.strip())
    for marker in ('MERGE_HEAD', 'CHERRY_PICK_HEAD', 'REVERT_HEAD', 'rebase-merge', 'rebase-apply', 'BISECT_LOG'):
        if (directory / marker).exists():
            raise ValueError('An existing Git operation needs local review.')
    if git(repo, 'ls-files', '--unmerged').stdout:
        raise ValueError('Existing conflicts need local review.')
    return dict(head=head, upstream=upstream, branch=branch, dirty=dirty, directory=directory)


def classify(repo):
    info = snapshot(repo)
    counts = git(repo, 'rev-list', '--left-right', '--count', 'HEAD...@{upstream}').stdout.split()
    ahead, behind = map(int, counts)
    lock = info['directory'] / 'index.lock'
    return info, dict(dirty=bool(info['dirty']), ahead=ahead, behind=behind,
                      index_lock=lock.exists(), error_class='index_lock' if lock.exists() else
                      'dirty_worktree' if info['dirty'] else 'diverged' if ahead and behind else 'retry_pull')


def advise(facts, taskdir):
    """No paths, Git output, file contents or credentials are sent to Codex."""
    executable = shutil.which('codex')
    if not executable:
        raise ValueError('Codex is unavailable; no agent repair was attempted.')
    schema = dict(type='object', properties={'action': dict(type='string', enum=list(ACTIONS))},
                  required=['action'], additionalProperties=False)
    schema_path = taskdir/'schema.json'; output = taskdir/'answer.json'
    schema_path.write_text(json.dumps(schema))
    prompt = ('Choose one permitted Git recovery action from the facts below. Do not use tools, '
              'read any other files, access accounts, or make changes. dirty worktrees require needs_user; '
              'index_lock suggests stale_index_lock (the executor verifies safety); clean divergence '
              'suggests merge_upstream (executor tests conflicts separately); otherwise retry. '
              'Return only the schema object. Facts: ' + json.dumps(facts))
    process = subprocess.Popen([executable, 'exec', '--ignore-user-config', '--sandbox', 'read-only', '--ephemeral',
                                '-c', 'approval_policy="never"', '--skip-git-repo-check',
                                '--cd', str(taskdir), '--output-schema', str(schema_path),
                                '--output-last-message', str(output), '-'],
                               stdin=subprocess.PIPE, text=True, stdout=subprocess.DEVNULL,
                               stderr=subprocess.DEVNULL, start_new_session=True)
    try:
        process.communicate(prompt, timeout=90)
    except subprocess.TimeoutExpired:
        os.killpg(process.pid, signal.SIGKILL)
        process.wait()
        raise
    if process.returncode or not output.exists() or output.stat().st_size > 1024:
        raise ValueError('Codex diagnosis failed or timed out; no repair selected.')
    answer = json.loads(output.read_text())
    if set(answer) != {'action'} or answer['action'] not in ACTIONS:
        raise ValueError('Codex returned an invalid action; no repair selected.')
    return answer['action']


def same_snapshot(repo, before):
    after = snapshot(repo)
    if any(after[k] != before[k] for k in ('head', 'upstream', 'branch', 'dirty')):
        raise ValueError('Repository changed while repair was running; leaving it alone.')


def retire_lock(repo, before, backup_dir):
    lock = before['directory']/'index.lock'
    meta = lock.lstat()
    if (not stat.S_ISREG(meta.st_mode) or meta.st_uid != os.getuid() or meta.st_nlink != 1
            or meta.st_size != 0 or time.time()-meta.st_mtime < 600):
        raise ValueError('Index lock is fresh, nonempty or not privately owned; local review needed.')
    running = subprocess.run(['ps', '-U', str(os.getuid()), '-o', 'comm='],
                             capture_output=True, text=True, timeout=5, check=True).stdout.splitlines()
    if any(Path(name.strip()).name == 'git' or Path(name.strip()).name.startswith('git-') for name in running):
        raise ValueError('A Git process is active; index lock kept.')
    same_snapshot(repo, before)
    current = lock.lstat()
    if (current.st_dev, current.st_ino, current.st_mtime_ns) != (meta.st_dev, meta.st_ino, meta.st_mtime_ns):
        raise ValueError('Index lock changed; leaving it alone.')
    backup_dir.mkdir(parents=True, exist_ok=True, mode=0o700)
    destination = backup_dir/('index.lock-'+str(time.time_ns()))
    lock.rename(destination)
    destination.chmod(0o600)


def merge_clean(repo, before, taskdir):
    if before['dirty']:
        raise ValueError('Local edits/untracked files need review; no automatic stash or commit.')
    worktree = taskdir/'merge-worktree'
    git(repo, 'worktree', 'add', '--detach', str(worktree), before['head'])
    try:
        result = git(worktree, '-c', 'core.hooksPath=/dev/null', '-c', 'commit.gpgsign=false',
                     'merge', '--no-edit', before['upstream'], check=False)
        if result.returncode:
            raise ValueError('Merge conflicts or merge failure in isolated worktree; original worktree unchanged.')
        merged = git(worktree, 'rev-parse', 'HEAD').stdout.strip()
        same_snapshot(repo, before)
        git(repo, '-c', 'core.hooksPath=/dev/null', 'merge', '--ff-only', merged)
    finally:
        git(repo, 'worktree', 'remove', '--force', str(worktree), check=False)


def repair(repo, state):
    """Return whether a verified fast-forward pull succeeds; never pushes."""
    repo = repo.resolve(strict=True)
    state.mkdir(parents=True, exist_ok=True, mode=0o700)
    result = dict(ok=False, reason='Repair did not finish.')
    try:
        lock_path = state.parent/('repo-'+hashlib.sha256(str(repo).encode()).hexdigest()+'.lock')
        with lock_path.open('a') as lock:
            fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
            git(repo, 'fetch', '--no-tags')
            before, facts = classify(repo)
            with tempfile.TemporaryDirectory(prefix='dailylogin-git-', dir='/tmp') as tmp:
                taskdir = Path(tmp)
                action = advise(facts, taskdir)
                same_snapshot(repo, before)
                if action == 'needs_user' or before['dirty']:
                    raise ValueError('Local changes or a complex Git state need review; no files discarded or stashed.')
                if action == 'stale_index_lock':
                    retire_lock(repo, before, state/'backups')
                elif action == 'merge_upstream':
                    merge_clean(repo, before, taskdir)
                git(repo, 'pull', '--no-rebase', '--ff-only')
                result = dict(ok=True, reason='Repair completed and pull retry succeeded; main sync must still be retried.')
    except ValueError as error:
        result['reason'] = str(error)
    except subprocess.TimeoutExpired:
        result['reason'] = 'Repair deadline exceeded; retry manually. No forced reset or push performed.'
    except subprocess.CalledProcessError:
        result['reason'] = 'Git command failed (network, permissions or repository state); local review needed.'
    except (OSError, BlockingIOError):
        result['reason'] = 'Storage/process/lock unavailable; local work kept.'
    temporary = state/'result.tmp';temporary.write_text(json.dumps(result));temporary.chmod(0o600)
    temporary.replace(state/'result.json')
    return result


def main():
    os.umask(0o077)
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, required=True)
    parser.add_argument('--state', type=Path, required=True)
    args = parser.parse_args()
    result = repair(args.repo, args.state)
    print(result['reason'])
    return 0 if result['ok'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
