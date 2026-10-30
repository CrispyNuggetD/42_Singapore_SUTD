#!/usr/bin/env python3
"""Private lock backup regression checks using disposable local Git repos."""
import contextlib
import io
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch
import lock_backup
import setup_private_backup


def git(repo, *args):
    return subprocess.check_output(['git', '-C', str(repo), *args], stderr=subprocess.DEVNULL)

class BackupTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        root = Path(self.temp.name)
        self.source = root / 'school'
        self.source.mkdir()
        git(self.source, 'init')
        git(self.source, 'config', 'user.name', 'Test')
        git(self.source, 'config', 'user.email', 'test@example.com')
        (self.source / 'main.c').write_text('original\n')
        (self.source / '.gitignore').write_text('*.bin\n')
        (self.source / 'tracked.bin').write_text('tracked\n')
        git(self.source, 'add', '-f', 'tracked.bin')
        git(self.source, 'add', '.')
        git(self.source, 'commit', '-m', 'initial')
        self.remote = root / 'private.git'
        subprocess.run(['git', 'init', '--bare', str(self.remote)], check=True,
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        self.config = dict(enabled=True, private_confirmed=True, source=str(self.source),
                           paths=['.'], storage=str(root / 'state'), remote=str(self.remote), branch='wip/test')

    def save(self):
        with contextlib.redirect_stdout(io.StringIO()):
            lock_backup.checkpoint(self.config)

    def test_snapshot_preserves_working_branch_and_index(self):
        (self.source / 'main.c').write_text('WIP\n')
        git(self.source, 'add', 'main.c')
        (self.source / 'new.txt').write_text('new\n')
        (self.source / 'ignored.bin').write_text('ignored\n')
        (self.source / 'link').symlink_to('main.c')
        index = (self.source / '.git/index').read_bytes()
        head = git(self.source, 'rev-parse', 'HEAD')
        self.save()
        self.assertEqual(Path(self.config['storage']).stat().st_mode&0o777,0o700)
        names = git(self.remote, 'ls-tree', '-r', '--name-only', 'wip/test').decode().splitlines()
        self.assertEqual(names, ['.gitignore', 'link', 'main.c', 'new.txt', 'tracked.bin'])
        self.assertTrue(git(self.remote, 'ls-tree', 'wip/test', 'link').startswith(b'120000'))
        self.assertEqual(index, (self.source / '.git/index').read_bytes())
        self.assertEqual(head, git(self.source, 'rev-parse', 'HEAD'))
        first = git(self.remote, 'rev-parse', 'wip/test')
        self.save()
        self.assertEqual(first, git(self.remote, 'rev-parse', 'wip/test'))
        (self.source / 'new.txt').unlink()
        self.save()
        self.assertNotIn('new.txt', git(self.remote, 'ls-tree', '-r', '--name-only', 'wip/test').decode().splitlines())

    def test_large_file_is_compressed_losslessly_and_push_retry_is_stable(self):
        import gzip
        data=b'backup contents\n'*10000
        (self.source/'large.json').write_bytes(data)
        with patch.object(lock_backup,'MAX_BLOB',1024):
            self.save()
            first=git(self.remote,'rev-parse','wip/test')
            self.save()
        zipped=git(self.remote,'show','wip/test:large.json.42-backup.gz')
        self.assertEqual(gzip.decompress(zipped),data)
        self.assertEqual(first,git(self.remote,'rev-parse','wip/test'))
        self.assertEqual((self.source/'large.json').read_bytes(),data)

    def test_compressed_codex_rollouts_are_backed_up_as_jsonl(self):
        import shutil
        import leaveschool
        if not shutil.which('zstd'):self.skipTest('zstd unavailable')
        git(self.source,'remote','add','origin','git@github.com:CrispyNuggetD/42_Singapore_SUTD_hnah_private.git')
        sessions=Path(self.temp.name)/'sessions'
        sessions.mkdir()
        raw=sessions/'rollout.jsonl'
        content=b'{"type":"session_meta","payload":{}}\n'
        raw.write_bytes(content)
        with raw.with_suffix('.jsonl.zst').open('wb') as output:
            subprocess.run(['zstd','-q','-c',str(raw)],stdout=output,check=True)
        raw.unlink()
        leaveschool.backup(sessions,self.source,Path(self.temp.name)/'other-repo')
        self.assertEqual((self.source/'42/codex_cache/sessions/rollout.jsonl').read_bytes(),content)

    def test_explicit_ignored_large_file_is_included_without_changing_gitignore(self):
        import gzip
        (self.source/'ignored.bin').write_bytes(b'latest raw data\n'*10000)
        self.config['extra_files']=['ignored.bin']
        with patch.object(lock_backup,'MAX_BLOB',1024):self.save()
        raw=git(self.remote,'show','wip/test:ignored.bin.42-backup.gz')
        self.assertEqual(gzip.decompress(raw),(self.source/'ignored.bin').read_bytes())
        self.assertEqual(git(self.source,'check-ignore','ignored.bin').strip(),b'ignored.bin')
        self.config['extra_files']=['missing-explicit-file']
        with self.assertRaisesRegex(ValueError,'Explicit backup file is missing'):self.save()
        self.config['extra_files']=['../outside']
        with self.assertRaisesRegex(ValueError,'Invalid Git file path'):self.save()

    def test_settings_snapshot_is_separate_and_rejects_auth_files(self):
        git(self.source,'remote','add','origin','git@github.com:CrispyNuggetD/42_Singapore_SUTD_hnah_private.git')
        folder=self.source/'42/lan42';folder.mkdir(parents=True)
        (folder/'login-settings.json').write_text('{"close_tab":false}')
        config=dict(self.config,settings_backup_private=str(self.source),settings_backup_remote=str(self.remote),
                    extra_files=['main.c'])
        with contextlib.redirect_stdout(io.StringIO()):lock_backup.backup_preferences(config)
        names=git(self.remote,'ls-tree','-r','--name-only','wip/42-lan42-settings')
        self.assertEqual(names,b'42/lan42/login-settings.json\n')
        (folder/'auth-server.json').write_text('PRIVATE TEST SENTINEL')
        with self.assertRaisesRegex(ValueError,'Unrecognized file'):
            lock_backup.backup_preferences(config)

    def test_watcher_failure_warns_without_exposing_file_contents(self):
        import ast
        from unittest.mock import Mock
        text=(Path(__file__).resolve().parents[1]/'tools/lock_backup_watch.py').read_text()
        node=next(n for n in ast.parse(text).body if isinstance(n,ast.FunctionDef) and n.name=='finished')
        process=Mock()
        scope=dict(os=__import__('os'),subprocess=Mock(),note=Mock(),child=None,queued=False)
        exec(compile(ast.Module(body=[node],type_ignores=[]),'<watcher>','exec'),scope)
        scope['finished'](1,256)
        command=scope['subprocess'].Popen.call_args.args[0]
        self.assertIn('Backup incomplete',command)
        scope['subprocess'].Popen.reset_mock()
        scope['finished'](1,0)
        scope['subprocess'].Popen.assert_not_called()

    def test_lan42_code_backup_excludes_unrelated_files_and_preserves_index(self):
        git(self.source,'remote','add','origin','https://github.com/CrispyNuggetD/42sg-campus-lan.git')
        (self.source/'campus_lan').mkdir()
        (self.source/'campus_lan/change.py').write_text('print("test fixture")\n')
        before=(self.source/'.git/index').read_bytes()
        with contextlib.redirect_stdout(io.StringIO()):
            lock_backup.backup_lan42_source(dict(self.config,lan42_source=str(self.source)))
        self.assertEqual(git(self.remote,'ls-tree','-r','--name-only','wip/42-lan42-source'),b'campus_lan/change.py\n')
        self.assertEqual((self.source/'.git/index').read_bytes(),before)

    def test_credentials_stop_checkpoint(self):
        (self.source / '.env').write_text('dummy test fixture')
        with self.assertRaisesRegex(ValueError, 'Credential-like'):
            self.save()
        self.assertFalse(Path(self.config['storage']).exists())

    def test_unconfirmed_private_destination_is_rejected(self):
        self.config['private_confirmed'] = False
        with self.assertRaisesRegex(ValueError, 'not been confirmed'):
            self.save()

    def test_chat_snapshot_includes_only_transcripts(self):
        config = dict(self.config, paths=['chats'])
        (self.source / 'chats').mkdir()
        (self.source / 'chats/session.jsonl').write_text('{"test":true}\n')
        with contextlib.redirect_stdout(io.StringIO()):
            lock_backup.checkpoint(config)
        self.assertEqual(git(self.remote, 'ls-tree', '-r', '--name-only', 'wip/test'), b'chats/session.jsonl\n')

    @patch('setup_private_backup.sys.platform', 'linux')
    def test_public_destination_is_rejected_by_installer(self):
        with patch('setup_private_backup.run', return_value=(str(self.source)+'\n').encode()), \
             patch('setup_private_backup.subprocess.run') as command, \
             patch.dict('os.environ', {'XDG_RUNTIME_DIR': self.temp.name}):
            command.return_value.stdout = '200'
            with self.assertRaisesRegex(ValueError, 'public'):
                setup_private_backup.install(str(self.source), 'git@github.com:someone/public.git', confirmed=True)

    def test_lock_signals_are_deduplicated_and_return_rearms(self):
        text = ((Path(__file__).resolve().parents[1]/'tools/lock_backup_watch.py')).read_text()
        definition = text[text.index('def evaluate():'):text.index('\ndef properties_changed')]
        events = []
        scope = dict(active=True, locked=False, screen_locked=False, fired=False,
                     launch=lambda: events.append('save'), note=lambda _: None)
        exec(definition, scope)
        scope['active'] = False; scope['evaluate']()
        scope['locked'] = True; scope['evaluate']()
        self.assertEqual(events, ['save'])
        scope['active'] = True; scope['locked'] = False; scope['evaluate']()
        scope['screen_locked'] = True; scope['evaluate']()
        self.assertEqual(events, ['save', 'save'])

if __name__ == '__main__':
    unittest.main()
