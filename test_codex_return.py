"""Round-trip tests use temporary repositories/Codex homes and no model calls."""
import contextlib
import importlib.util
import io
import json
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace

import codex_return as handoff

HELPER = Path(__file__).resolve().parent.parent / '42_Singapore_SUTD_hnah_private/Scripts and Testers/codex_school_import.py'


@unittest.skipUnless(shutil.which('codex') and HELPER.exists(), 'Codex and private helper required')
class RoundTrip(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name).resolve()
        self.private = self.root/'mac-repos/42_Singapore_SUTD_hnah_private'
        (self.private/'42').mkdir(parents=True)
        self.main = self.private.parent/'42_Singapore_SUTD'
        self.project = self.main/'Core Curriculum/Projects/2_pipex'
        self.project.mkdir(parents=True)
        (self.private/'Scripts and Testers').mkdir()
        shutil.copy2(HELPER, self.private/'Scripts and Testers/codex_school_import.py')
        self.mod = handoff.helper(self.private)
        self.home = self.root/'mac-codex'
        self.source_id = '01a0af97-56b2-7f33-bef9-e74eb953cf61'
        self.source = self.private/('42/codex_cache/sessions/2026/09/17/rollout-2026-09-17T13-38-46-'+self.source_id+'.jsonl')
        self.source.parent.mkdir(parents=True)
        self.rows = [{'timestamp': '2026-09-17T13:38:46Z', 'type': 'session_meta', 'payload': {
            'id': self.source_id, 'timestamp': '2026-09-17T13:38:46Z',
            'cwd': '/home/hnah/Documents/42_Singapore_SUTD/Core Curriculum/Projects/2_pipex',
            'source': 'vscode', 'originator': 'codex_vscode', 'model_provider': 'openai',
            'cli_version': '0.154.0', 'history_mode': 'legacy'}},
            self.message('School question')]
        self.source.write_text(self.encode(self.rows))
        with contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(self.mod.run(SimpleNamespace(private=self.private, codex_home=self.home, codex='codex')), 0)
        self.entry = next(iter(json.loads((self.home/'school-handoff/manifest.json').read_text())['imports'].values()))
        self.rollout = Path(self.entry['rollout'])

    @staticmethod
    def message(text):
        return {'timestamp': '2026-09-17T14:00:00Z', 'type': 'response_item', 'payload': {
            'type': 'message', 'role': 'user', 'content': [{'type': 'input_text', 'text': text}]}}

    @staticmethod
    def encode(rows):
        return ''.join(json.dumps(row)+'\n' for row in rows)

    def append(self, text):
        with self.rollout.open('a') as stream:
            stream.write(self.encode([self.message(text)]))

    def quiet(self, fn, *args, **kwargs):
        with contextlib.redirect_stdout(io.StringIO()):
            return fn(*args, **kwargs)

    def test_round_trip_excludes_mac_only_chats_and_preserves_existing_school(self):
        # A completely unrelated Mac-only chat must never be enumerated/exported.
        (self.home/'sessions/mac-only.jsonl').write_text('PRIVATE MAC-ONLY SENTINEL')
        self.assertEqual(self.quiet(handoff.export_home, self.private, self.home), 0)
        self.append('Continuation written on Mac')
        self.assertEqual(self.quiet(handoff.export_home, self.private, self.home, check=True), 1)
        self.assertFalse((self.private/handoff.RETURN_ROOT/'index.json').exists())
        self.assertEqual(self.quiet(handoff.export_home, self.private, self.home), 1)
        self.assertEqual(self.quiet(handoff.export_home, self.private, self.home), 0)
        index = json.loads((self.private/handoff.RETURN_ROOT/'index.json').read_text())
        self.assertEqual(len(index['snapshots']), 1)
        school = self.root/'school-codex'
        school_main = self.root/'school-repos/42_Singapore_SUTD'
        school_project = school_main/'Core Curriculum/Projects/2_pipex'
        school_project.mkdir(parents=True)
        existing = school/'sessions/original-school.jsonl'
        existing.parent.mkdir(parents=True)
        existing.write_text('UNCHANGED SCHOOL SENTINEL')
        self.assertEqual(self.quiet(handoff.import_home, self.private, school, school_main, self.root/'school-home', 'codex'), 1)
        self.assertEqual(self.quiet(handoff.import_home, self.private, school, school_main, self.root/'school-home', 'codex'), 0)
        state = json.loads((school/'school-handoff/returns.json').read_text())
        returned = next(iter(state['imports'].values()))
        content = Path(returned['rollout']).read_text()
        self.assertIn('Continuation written on Mac', content)
        self.assertNotIn('PRIVATE MAC-ONLY SENTINEL', content)
        self.assertEqual(returned['cwd'], str(school_project))
        self.assertNotEqual(returned['school_id'], self.source_id)
        self.assertEqual(existing.read_text(), 'UNCHANGED SCHOOL SENTINEL')
        # A school backup of an unchanged returned copy must not echo into Mac.
        echo = self.source.parent/Path(returned['rollout']).name
        echo.write_text(content)
        self.assertEqual(self.quiet(self.mod.run, SimpleNamespace(private=self.private, codex_home=self.home, codex='codex')), 0)
        local = json.loads((self.home/'school-handoff/manifest.json').read_text())
        self.assertEqual(len(local['imports']), 1)
        # A second Mac revision becomes a different snapshot and preserves the first.
        self.append('A later Mac continuation')
        self.assertEqual(self.quiet(handoff.export_home, self.private, self.home), 1)
        self.assertEqual(self.quiet(handoff.import_home, self.private, school, school_main, self.root/'school-home', 'codex'), 1)
        self.assertEqual(Path(returned['rollout']).read_text(), content)

    def test_incomplete_or_active_chat_exports_nothing(self):
        self.append('New turn')
        with self.rollout.open('a') as stream:
            stream.write(self.encode([{'type': 'event_msg', 'payload': {'type': 'task_started', 'turn_id': 'busy'}}]))
        with self.assertRaisesRegex(ValueError, 'unfinished turn'):
            handoff.export_home(self.private, self.home)
        self.assertFalse((self.private/handoff.RETURN_ROOT/'index.json').exists())
        with self.rollout.open('a') as stream:
            stream.write('{partial')
        with self.assertRaises(ValueError):
            handoff.export_home(self.private, self.home)

    def test_chat_only_git_commit_preserves_other_staged_work(self):
        remote = self.root/'remote/42_Singapore_SUTD_hnah_private'
        remote.parent.mkdir()
        subprocess.run(['git', 'init', '--bare', str(remote)], check=True, capture_output=True)
        def git(*args): return handoff.git(self.private, *args)
        git('init', '-b', 'main')
        git('config', 'user.email', 'test@example.invalid')
        git('config', 'user.name', 'Handoff test')
        git('remote', 'add', 'origin', str(remote))
        git('add', '.')
        git('commit', '-m', 'Fixture')
        git('push', '-u', 'origin', 'main')
        other = self.private/'unrelated.txt'
        other.write_text('Keep staged')
        git('add', 'unrelated.txt')
        self.append('Changes to send')
        self.quiet(handoff.sync_export, self.private, self.home)
        self.assertEqual(git('diff', '--cached', '--name-only'), 'unrelated.txt')
        names = git('show', '--pretty=format:', '--name-only', 'HEAD').splitlines()
        self.assertTrue(all(name.startswith(handoff.RETURN_ROOT+'/') for name in names if name))
        self.assertEqual(git('rev-parse', 'HEAD'), git('rev-parse', 'origin/main'))
        git('commit', '-m', 'Unrelated local commit')
        with self.assertRaisesRegex(ValueError, 'non-chat commits'):
            handoff.sync_export(self.private, self.home)


if __name__ == '__main__':
    unittest.main()
