#!/usr/bin/python3
"""Save school repository files to an independent private WIP history."""
import argparse
from datetime import datetime
import fcntl
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

CONFIG = Path(os.environ.get('XDG_CONFIG_HOME', str(Path.home() / '.config'))) / '42-lock-checkpoint.json'

def run(args, cwd=None, capture=True):
    env = dict(os.environ, GIT_TERMINAL_PROMPT='0',
               GIT_SSH_COMMAND='ssh -o BatchMode=yes -o ConnectTimeout=10')
    return subprocess.run(args, cwd=cwd, env=env, check=True,
                          stdout=subprocess.PIPE if capture else None,
                          stderr=subprocess.PIPE, timeout=600).stdout

def selected(source, paths):
    data = run(['git', '-C', str(source), 'ls-files', '-z', '--cached', '--others',
                '--exclude-standard', '--', *paths])
    files = []
    for raw in sorted(set(data.split(b'\0')) - {b''}):
        relative = Path(os.fsdecode(raw))
        full = source / relative
        if relative.is_absolute() or '..' in relative.parts:
            raise ValueError('Invalid Git file path')
        # Never follow directory symlinks outside the repository.
        if any((source / Path(*relative.parts[:i])).is_symlink()
               for i in range(1, len(relative.parts))):
            raise ValueError('Refusing a path through a directory symlink')
        if full.is_file() or full.is_symlink():
            files.append(relative)
    return files

def checkpoint(config, preview=False):
    if not config.get('private_confirmed'):
        raise ValueError('Private destination has not been confirmed by setupprivatebackup')
    if not config.get('enabled'):
        print('Screen-lock checkpoint disabled')
        return
    source = Path(config['source']).resolve(strict=True)
    files = selected(source, config['paths'])
    # Account privacy policy forbids exporting credentials, even to private Git.
    for relative in files:
        name = relative.name.lower()
        if name == '.env' or name.startswith('.env.') and not name.endswith(('example', 'sample')) or name in {'id_rsa', 'id_ed25519', 'credentials.json', 'auth.json', '.git-credentials'} or name.endswith(('.key', '.p12', '.pfx')):
            raise ValueError('Credential-like file found; checkpoint stopped for local review')
    if preview:
        print('\n'.join(map(str, files)))
        print(f'{len(files)} files; destination {config["remote"]}; branch {config["branch"]}')
        return
    if not files:
        raise ValueError('No files found; refusing an empty snapshot')
    storage = Path(config['storage']).expanduser()
    storage.mkdir(parents=True, exist_ok=True)
    with (storage / 'checkpoint.lock').open('w') as lock:
        try:
            fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except BlockingIOError:
            print('Another checkpoint is running')
            return
        gitdir = storage / 'history.git'
        if not gitdir.exists():
            run(['git', 'init', '--bare', str(gitdir)])
            for setting in ('user.name', 'user.email'):
                value = run(['git', '-C', str(source), 'config', setting]).decode().strip()
                run(['git', '--git-dir', str(gitdir), 'config', setting, value])
        git = ['git', '--git-dir', str(gitdir)]
        branch = config['branch']
        run(git + ['check-ref-format', 'refs/heads/' + branch])
        ref = 'refs/heads/' + branch
        # Fresh index contains only this snapshot; the school index is untouched.
        with tempfile.TemporaryDirectory(dir=storage) as tmp:
            tree_dir = Path(tmp)
            for relative in files:
                target = tree_dir / relative
                target.parent.mkdir(parents=True, exist_ok=True)
                if (source / relative).is_symlink():
                    target.symlink_to(os.readlink(source / relative))
                else:
                    shutil.copy2(source / relative, target)
            run(git + ['read-tree', '--empty'])
            run(git + ['--work-tree', str(tree_dir), 'add', '--force', '--all', '--', '.'], cwd=tree_dir)
            tree = run(git + ['write-tree']).decode().strip()
        try:
            parent = run(git + ['rev-parse', '--verify', ref]).decode().strip()
        except subprocess.CalledProcessError:
            parent = None
        if parent and run(git + ['rev-parse', parent + '^{tree}']).decode().strip() == tree:
            print('No file changes; retrying push of existing checkpoint')
        else:
            message = 'WIP checkpoint: screen lock — ' + datetime.now().astimezone().isoformat(timespec='seconds')
            args = git + ['commit-tree', tree, '-m', message]
            if parent:
                args += ['-p', parent]
            commit = run(args).decode().strip()
            run(git + ['update-ref', ref, commit])
            print('Saved local checkpoint ' + commit)
        # Exact destination and branch; never push the public origin or force.
        run(git + ['push', config['remote'], ref + ':' + ref])
        print('Private WIP push succeeded', flush=True)

def backup_chats(config):
    private = config.get('chat_backup_private')
    if not private:
        return
    private = Path(private).expanduser().resolve(strict=True)
    # Owner's chat export is authorized only to this separate private repo.
    from leaveschool import backup
    source = Path(os.environ.get('CODEX_HOME', str(Path.home() / '.codex'))) / 'sessions'
    backup(source, private, Path(config['source']))
    # Do not publish unrelated unpushed commits from the personal working branch.
    # An independent backup branch snapshots only the authorized transcript folder.
    chats = dict(config, source=str(private), paths=['42/codex_cache'],
                 storage=str(Path(config['storage']) / 'chats'),
                 remote=config['chat_backup_remote'], branch='wip/42-chat-backup',
                 chat_backup_private=None)
    checkpoint(chats)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--preview', action='store_true')
    args = parser.parse_args()
    config = json.loads(CONFIG.read_text())
    failed = False
    for action in [lambda: checkpoint(config, args.preview)] + ([] if args.preview or not config.get('enabled') else [lambda: backup_chats(config)]):
        try:
            action()
        except Exception as error:
            failed = True
            print('Checkpoint failed: ' + str(error), flush=True)
            if isinstance(error, subprocess.CalledProcessError) and error.stderr:
                print(error.stderr.decode(errors='replace'), flush=True)
    raise SystemExit(int(failed))
