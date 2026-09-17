#!/usr/bin/env python3
"""Return only imported school chats from Mac to the private repo, then Ubuntu.

Chat payloads stay in 42/codex_cache/return_to_school. Native Codex migration
creates separate resumable snapshots; existing school threads are never replaced.
"""
import argparse
import fcntl
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import uuid

RETURN_ROOT = '42/codex_cache/return_to_school'
NAMESPACE = uuid.UUID('ea37343e-70bd-48aa-83f0-a415a326a2e0')


def git(repo, *args):
    env = dict(os.environ, GIT_TERMINAL_PROMPT='0')
    env['GIT_SSH_COMMAND'] = env.get('GIT_SSH_COMMAND', 'ssh') + ' -o BatchMode=yes -o ConnectTimeout=15'
    return subprocess.run(['git', '-C', str(repo), *args], env=env, check=True,
                          text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE).stdout.strip()


def validate_private(private):
    private = private.resolve(strict=True)
    top = Path(git(private, 'rev-parse', '--show-toplevel')).resolve()
    remote = git(private, 'remote', 'get-url', 'origin')
    if top != private or not remote.removesuffix('.git').endswith('/42_Singapore_SUTD_hnah_private'):
        raise ValueError('Expected the separate 42_Singapore_SUTD_hnah_private repository')
    target = private / RETURN_ROOT
    if target.resolve() != target:
        raise ValueError('Refusing a symlinked return destination')
    return private


def helper(private):
    source = private / 'Scripts and Testers/codex_school_import.py'
    spec = importlib.util.spec_from_file_location('school_import', source)
    module = importlib.util.module_from_spec(spec)
    previous = sys.dont_write_bytecode
    try:
        sys.dont_write_bytecode = True
        spec.loader.exec_module(module)
    finally:
        sys.dont_write_bytecode = previous
    if not hasattr(module, 'history_digest'):
        raise ValueError('Pull the private repository to update codex_school_import.py first')
    return module


def load(path, default):
    return json.loads(path.read_text()) if path.exists() else default


def confined(root, relative):
    result = root / relative
    if result.is_symlink() or result.resolve() != result or root not in result.resolve().parents:
        raise ValueError('Refusing an unsafe handoff path')
    return result


def read_snapshot(path, home):
    if path.is_symlink() or home not in path.resolve().parents:
        raise ValueError('Chat rollout must be inside CODEX_HOME')
    before = path.stat()
    raw = path.read_bytes()
    after = path.stat()
    if (before.st_size, before.st_mtime_ns) != (after.st_size, after.st_mtime_ns):
        raise ValueError('Chat is changing; wait for its current turn to finish and retry')
    rows = [json.loads(line) for line in raw.decode().split('\n') if line.strip()]
    if not rows or rows[0].get('type') != 'session_meta':
        raise ValueError('Missing session metadata')
    return raw, rows


def active_turn(rows):
    pending = None
    for row in rows:
        payload = row.get('payload', {})
        if row.get('type') != 'event_msg' or not isinstance(payload, dict):
            continue
        kind = payload.get('type')
        turn = payload.get('turn_id', 'current')
        if kind == 'task_started':
            pending = turn
        elif kind in ('task_complete', 'turn_aborted') and (turn == pending or turn == 'current'):
            pending = None
    return bool(pending)


def export_home(private, home, check=False):
    mod = helper(private)
    state_path = home / 'school-handoff/manifest.json'
    state = load(state_path, {'imports': {}})
    target = private / RETURN_ROOT
    index_path = target / 'index.json'
    index = load(index_path, {'version': 1, 'snapshots': {}})
    pending = []
    unchanged = 0
    # Strict allowlist: never scan arbitrary Mac conversations or copy all .codex.
    for entry in state['imports'].values():
        local_id = str(uuid.UUID(entry['local_id']))
        path = Path(entry['rollout'])
        if not path.exists():
            archived = list((home / 'archived_sessions').glob(f'**/*{local_id}.jsonl'))
            if len(archived) != 1:
                raise ValueError(f'Imported chat {local_id} is missing; restore it or remove its manifest entry')
            path = archived[0]
        raw, rows = read_snapshot(path, home)
        if rows[0]['payload'].get('id') != local_id:
            raise ValueError('Manifest/rollout ID mismatch')
        baseline = entry.get('baseline_history_sha256')
        if not baseline:
            original = confined(private / '42/codex_cache/sessions', entry['source']).read_bytes()
            if hashlib.sha256(original).hexdigest() != entry['sha256']:
                raise ValueError(f'Cannot reconstruct the import baseline for {local_id}; original school snapshot changed')
            baseline = mod.history_digest(original)
            entry['baseline_history_sha256'] = baseline
        digest = mod.history_digest(raw)
        if digest == baseline:
            unchanged += 1
            continue
        if active_turn(rows):
            raise ValueError(f'Chat {local_id} has an unfinished turn; finish/stop it in Codex before leavehome')
        school_id = str(uuid.uuid5(NAMESPACE, local_id + ':' + digest))
        if school_id in index['snapshots']:
            unchanged += 1
            continue
        name = path.name.replace(local_id, school_id)
        if name == path.name:
            raise ValueError('Rollout filename does not contain its thread ID')
        relative = Path('sessions') / school_id / name
        record = {'mac_id': local_id, 'school_source_id': entry['source_id'],
                  'school_id': school_id, 'school_cwd': entry['school_cwd'],
                  'mac_cwd': entry['cwd'], 'mac_repos': str(private.parent),
                  'history_sha256': digest, 'sha256': hashlib.sha256(raw).hexdigest(),
                  'path': str(relative)}
        pending.append((confined(target, relative), raw, school_id, record))
    # Validate every selected chat before publishing any snapshot or index.
    if not check:
        for path, raw, school_id, record in pending:
            mod.atomic_write(path, raw.decode())
            index['snapshots'][school_id] = record
        if pending:
            mod.atomic_write(index_path, json.dumps(index, indent=2) + '\n')
        if state_path.exists():
            mod.atomic_write(state_path, json.dumps(state, indent=2) + '\n')
    print(f'leavehome: {len(pending)} changed school chats {"ready" if check else "exported"}; '
          f'{unchanged} unchanged; Mac-only chats excluded.')
    return len(pending)


def school_mapper(record, private, main, school_home):
    repos = {'42_Singapore_SUTD': main,
             '42_Singapore_SUTD_hnah_private': private,
             '42sg-campus-lan': main.parent / '42sg-campus-lan'}
    def translate(cwd):
        for prefix in (record['mac_repos'].rstrip('/') + '/', '/home/hnah/Documents/'):
            if cwd.startswith(prefix):
                relative = Path(cwd[len(prefix):])
                if relative.parts and relative.parts[0] in repos:
                    candidate = repos[relative.parts[0]].joinpath(*relative.parts[1:]).resolve()
                    root = repos[relative.parts[0]].resolve()
                    if (candidate == root or root in candidate.parents) and candidate.is_dir():
                        return str(candidate)
        if cwd in ('/home/hnah', '/home/hnah/Desktop'):
            candidate = school_home / ('Desktop' if cwd.endswith('/Desktop') else '')
            if candidate.is_dir():
                return str(candidate)
        return str(private / '42')
    def mapped(cwd):
        # Recover the original school Home/Desktop path collapsed during Mac import.
        if cwd == record['mac_cwd']:
            return translate(record['school_cwd'])
        return translate(cwd)
    return mapped


def import_home(private, home, main, school_home, codex):
    mod = helper(private)
    target = private / RETURN_ROOT
    index = load(target / 'index.json', {'snapshots': {}})
    state_path = home / 'school-handoff/returns.json'
    state = load(state_path, {'version': 1, 'imports': {}})
    added = skipped = 0
    for school_id, record in index['snapshots'].items():
        if str(uuid.UUID(school_id)) != record['school_id']:
            raise ValueError('Return index thread ID mismatch')
        if school_id in state['imports']:
            skipped += 1
            continue
        src = confined(target, record['path'])
        raw = src.read_bytes()
        if hashlib.sha256(raw).hexdigest() != record['sha256']:
            raise ValueError('Return snapshot checksum mismatch')
        if mod.history_digest(raw) != record['history_sha256']:
            raise ValueError('Return snapshot history checksum mismatch')
        mapper = school_mapper(record, private, main, school_home)
        data, source_id, cwd, original = mod.convert(raw, school_id, private.parent, private, mapper=mapper)
        if source_id != record['mac_id']:
            raise ValueError('Return snapshot source ID mismatch')
        expected = str(uuid.uuid5(NAMESPACE, source_id + ':' + record['history_sha256']))
        if expected != school_id or school_id not in src.name:
            raise ValueError('Return snapshot identity mismatch')
        dst = confined(home, Path('sessions/returned-from-home') / src.name)
        if not dst.exists():
            mod.atomic_write(dst, data)
        # This read registers metadata only; no model turn is started.
        mod.register(codex, home, school_id)
        result = subprocess.run([codex, 'migrate-rollouts', '--apply', '--thread', school_id, '--json'],
            env=dict(os.environ, CODEX_HOME=str(home)), capture_output=True, text=True, timeout=120)
        if result.returncode:
            raise ValueError('Native return migration failed: ' + result.stderr[-500:])
        outcomes = json.loads(result.stdout).get('outcomes', [])
        if not outcomes or any(item['status'] not in ('migrated', 'already_paginated') for item in outcomes):
            raise ValueError('Native return migration did not complete')
        state['imports'][school_id] = dict(record, cwd=cwd, rollout=str(dst))
        mod.atomic_write(state_path, json.dumps(state, indent=2) + '\n')
        added += 1
    print(f'School return import: {added} imported; {skipped} already present. '
          'Use Codex history; reload the VS Code window if needed.')
    return added


def sync_export(private, home):
    # A transcript-only command must not accidentally publish unrelated local commits.
    branch = git(private, 'symbolic-ref', '--quiet', '--short', 'HEAD')
    upstream = git(private, 'rev-parse', '--abbrev-ref', '--symbolic-full-name', '@{upstream}')
    if not upstream.startswith('origin/'):
        raise ValueError('leavehome requires a branch tracking origin in the private repo')
    for marker in ('MERGE_HEAD', 'CHERRY_PICK_HEAD', 'REVERT_HEAD', 'rebase-merge', 'rebase-apply'):
        path = Path(git(private, 'rev-parse', '--git-path', marker))
        if (path if path.is_absolute() else private / path).exists():
            raise ValueError('Finish the current Git operation before leavehome')
    git(private, 'fetch', 'origin')
    for commit in git(private, 'rev-list', upstream + '..HEAD').splitlines():
        names = git(private, 'diff-tree', '--no-commit-id', '--name-only', '-r', commit).splitlines()
        if not names or any(not name.startswith(RETURN_ROOT + '/') for name in names):
            raise ValueError('Unpublished non-chat commits exist; publish/review those separately before leavehome')
    git(private, '-c', 'merge.autostash=false', '-c', 'rebase.autostash=false',
        'pull', '--no-rebase', '--ff-only')
    export_home(private, home)
    if (private / RETURN_ROOT).exists():
        git(private, 'add', '--', RETURN_ROOT)
        if git(private, 'diff', '--cached', '--name-only', '--', RETURN_ROOT):
            git(private, 'commit', '--only', '-m', 'leavehome: return school Codex continuations', '--', RETURN_ROOT)
    git(private, 'push', 'origin', 'HEAD:refs/heads/' + upstream[len('origin/'):])
    print(f'leavehome: private {branch} chat handoff synced to origin. Applications remain open.')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('action', choices=('export', 'check', 'sync', 'import'))
    parser.add_argument('--private', type=Path, default=Path(os.environ.get('CODEX_PRIVATE_REPO_ROOT', Path.home()/'Documents/42_Singapore_SUTD_hnah_private')))
    parser.add_argument('--codex-home', type=Path, default=Path(os.environ.get('CODEX_HOME', Path.home()/'.codex')))
    parser.add_argument('--main-repo', type=Path, default=Path(os.environ.get('MAIN_REPO_ROOT', Path.home()/'Documents/42_Singapore_SUTD')))
    parser.add_argument('--codex', default=os.environ.get('CODEX_BIN', 'codex'))
    args = parser.parse_args()
    try:
        private = validate_private(args.private)
        home = args.codex_home.resolve()
        state_dir = home / 'school-handoff'
        state_dir.mkdir(parents=True, exist_ok=True)
        with (state_dir / 'import.lock').open('a') as lock:
            fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
            if args.action == 'sync':
                sync_export(private, home)
            elif args.action in ('export', 'check'):
                export_home(private, home, check=args.action == 'check')
            else:
                codex = shutil.which(args.codex)
                if not codex:
                    raise ValueError('Codex CLI missing; set CODEX_BIN to the extension/CLI executable')
                import_home(private, home, args.main_repo.resolve(), Path.home(), codex)
        return 0
    except (OSError, ValueError, KeyError, subprocess.SubprocessError) as error:
        detail = getattr(error, 'stderr', None) or str(error)
        print(f'Codex return handoff failed: {detail}')
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
