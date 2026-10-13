#!/usr/bin/env python3
"""Verify debug/non-project commits stay separate from project work."""
import contextlib
import io
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch
import git_side_push
import leaveschool


def git(repo, *args):
    return subprocess.check_output(['git', '-C', str(repo), *args], stderr=subprocess.DEVNULL)

class SidePushTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory(); self.addCleanup(self.tmp.cleanup)
        self.repo = Path(self.tmp.name) / 'school'; self.repo.mkdir()
        git(self.repo, 'init'); git(self.repo, 'config', 'user.name', 'Test'); git(self.repo, 'config', 'user.email', 'test@example.com')
        self.project = self.repo / 'Core Curriculum/Projects/current'; self.project.mkdir(parents=True)
        self.debug = self.repo / git_side_push.DEBUG_PATH; self.debug.mkdir(parents=True)
        (self.project / 'main.c').write_text('original')
        (self.debug / 'result.txt').write_text('original')
        (self.repo / 'README.md').write_text('original')
        git(self.repo, 'add', '.'); git(self.repo, 'commit', '-m', 'initial')
        self.remote = Path(self.tmp.name) / 'remote.git'
        subprocess.run(['git', 'init', '--bare', str(self.remote)], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        git(self.repo, 'remote', 'add', 'origin', str(self.remote)); git(self.repo, 'push', '-u', 'origin', 'HEAD')

    def side(self):
        with contextlib.redirect_stdout(io.StringIO()):
            return git_side_push.side_push(self.repo)

    def test_separate_commit_preserves_staged_project(self):
        (self.project / 'main.c').write_text('project WIP'); git(self.repo, 'add', '.')
        (self.debug / 'result.txt').write_text('debug update')
        (self.repo / 'new [note].md').write_text('new nonproject')
        (self.repo / 'README.md').unlink()
        self.side()
        names = git(self.repo, 'show', '--format=', '--name-only', 'HEAD').decode().splitlines()
        self.assertEqual(set(names), {git_side_push.DEBUG_PATH + '/result.txt', 'new [note].md', 'README.md'})
        self.assertEqual(git(self.repo, 'diff', '--cached', '--name-only').decode().strip(), 'Core Curriculum/Projects/current/main.c')
        self.assertEqual(git(self.repo, 'show', 'HEAD:Core Curriculum/Projects/current/main.c'), b'original')
        self.assertEqual(git(self.repo, 'rev-parse', 'HEAD'), git(self.remote, 'rev-parse', 'HEAD'))

    def test_cross_boundary_rename_stays_for_main_commit(self):
        git(self.repo, 'mv', 'Core Curriculum/Projects/current/main.c', 'moved.c')
        (self.repo / 'new.md').write_text('nonproject')
        self.side()
        self.assertEqual(git(self.repo, 'show', '--format=', '--name-only', 'HEAD').decode().strip(), 'new.md')
        self.assertIn(b'moved.c', git(self.repo, 'diff', '--cached', '--name-only'))

    def test_failed_side_push_blocks_main_commit(self):
        (self.project / 'main.c').write_text('project')
        (self.repo / 'README.md').write_text('nonproject')
        git(self.repo, 'remote', 'set-url', 'origin', str(Path(self.tmp.name) / 'missing.git'))
        with patch.dict('os.environ', {'MAIN_REPO_ROOT': str(self.repo)}), contextlib.redirect_stdout(io.StringIO()):
            with self.assertRaises(subprocess.CalledProcessError):
                leaveschool.push(self.repo)
        self.assertEqual(git(self.repo, 'show', 'HEAD:Core Curriculum/Projects/current/main.c'), b'original')
        self.assertEqual(git(self.repo, 'log', '-1', '--format=%s').decode().strip(), 'Update debug output and non-project files')

    def test_leaveschool_makes_side_commit_then_project_commit(self):
        (self.project / 'main.c').write_text('project')
        (self.repo / 'README.md').write_text('nonproject')
        with patch.dict('os.environ', {'MAIN_REPO_ROOT': str(self.repo)}), contextlib.redirect_stdout(io.StringIO()):
            leaveschool.push(self.repo)
        self.assertEqual(git(self.repo, 'show', '--format=', '--name-only', 'HEAD~1').decode().strip(), 'README.md')
        self.assertEqual(git(self.repo, 'show', '--format=', '--name-only', 'HEAD').decode().strip(), 'Core Curriculum/Projects/current/main.c')

    def test_break_wip_context_runs_after_side_commit(self):
        (self.project / 'main.c').write_text('unfinished project')
        (self.repo / 'README.md').write_text('midday notes')
        def context(repo):
            self.assertEqual(git(repo, 'log', '-1', '--format=%s').decode().strip(),
                             'Update debug output and non-project files')
            self.assertEqual(git(repo, 'diff', '--cached', '--name-only').decode().strip(),
                             'Core Curriculum/Projects/current/main.c')
            return 'Project implementation remains in progress.'
        with patch.dict('os.environ', {'MAIN_REPO_ROOT': str(self.repo)}), \
                patch('leaveschool.wip_body', side_effect=context) as summarizer, \
                contextlib.redirect_stdout(io.StringIO()):
            leaveschool.push(self.repo, wip=True)
        summarizer.assert_called_once()
        message = git(self.repo, 'log', '-1', '--format=%B').decode()
        self.assertTrue(message.startswith('WIP: break checkpoint '))
        self.assertIn('Project implementation remains in progress.', message)
        self.assertEqual(git(self.repo, 'rev-parse', 'HEAD'), git(self.remote, 'rev-parse', 'HEAD'))

    def test_break_records_even_without_changes_and_continues_after_failure(self):
        state = Path(self.tmp.name) / 'state'
        with patch.dict('os.environ', {'XDG_STATE_HOME': str(state)}), \
                patch('leaveschool.backup', side_effect=ValueError('backup failed')), \
                patch('leaveschool.repositories', return_value=[self.repo]), \
                patch('leaveschool.push') as push, contextlib.redirect_stdout(io.StringIO()):
            result = leaveschool.break_checkpoint(self.repo.parent, self.repo, self.repo)
        import json
        entries = [json.loads(line) for line in (state / '42-shell/breaks.jsonl').read_text().splitlines()]
        self.assertEqual([entry['event'] for entry in entries], ['break-start', 'checkpoint-finished'])
        self.assertEqual(entries[-1]['failures'], 1)
        self.assertEqual(result, 1)
        push.assert_called_once_with(self.repo, wip=True)

if __name__ == '__main__':
    unittest.main()
