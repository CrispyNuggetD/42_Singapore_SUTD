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

    def test_public_destination_is_rejected_by_installer(self):
        with patch('setup_private_backup.run', return_value=(str(self.source)+'\n').encode()), \
             patch('setup_private_backup.subprocess.run') as command, \
             patch.dict('os.environ', {'XDG_RUNTIME_DIR': self.temp.name}):
            command.return_value.stdout = '200'
            with self.assertRaisesRegex(ValueError, 'public'):
                setup_private_backup.install(str(self.source), 'git@github.com:someone/public.git', confirmed=True)

    def test_lock_signals_are_deduplicated_and_return_rearms(self):
        text = (Path(__file__).with_name('lock_backup_watch.py')).read_text()
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
