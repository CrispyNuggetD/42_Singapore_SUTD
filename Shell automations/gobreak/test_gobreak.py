"""Run with: python3 -m unittest discover -s 'Shell automations/gobreak'"""
import contextlib
import io
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

SOURCE = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(SOURCE))
import gobreak
import install_gobreak
import sync_zshrc
import leaveschool


def git(repo, *args):
    return subprocess.check_output(['git', '-C', str(repo), *args], text=True, stderr=subprocess.DEVNULL).strip()


class GoBreakTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix='42-gobreak-test-')
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.docs = self.root / 'Documents'
        self.docs.mkdir()
        self.bin = self.root / 'bin'
        self.bin.mkdir()
        for tool in ('ft_lock', 'gnome-terminal'):
            path = self.bin / tool
            path.write_text('#!/bin/sh\nexit 0\n')
            path.chmod(0o755)
        # Never write the executing user's real GNOME favorites during tests.
        settings = self.bin / 'gsettings'
        settings.write_text('#!/bin/sh\nexit 1\n')
        settings.chmod(0o755)
        self.env = dict(HOME=str(self.root), XDG_DATA_HOME=str(self.root / 'data'),
                        PATH=str(self.bin) + os.pathsep + os.environ['PATH'],
                        GOBREAK_CODEX_CONTEXT='FALSE')
        self.environment = patch.dict(os.environ, self.env)
        self.environment.start()
        self.addCleanup(self.environment.stop)
        for name in ('GOBREAK_REPOS_ROOT', 'GOBREAK_INCLUDE_REPOS', 'GOBREAK_EXCLUDE_REPOS',
                     'DOCUMENTS_PUSH_EXCLUDE', 'MAC_REMOTE_ROOT', 'CODEX_PRIVATE_REPO_ROOT'):
            os.environ.pop(name, None)

    def repo(self, name):
        path = self.docs / name
        path.mkdir(parents=True)
        git(path, 'init')
        git(path, 'config', 'user.name', 'Test')
        git(path, 'config', 'user.email', 'test@example.invalid')
        (path / 'work').write_text('initial\n')
        git(path, 'add', '.')
        git(path, 'commit', '-m', 'Initial')
        return path

    def test_selection_defaults_includes_exclusions_and_nested_order(self):
        one = self.repo('one')
        two = self.repo('two')
        nested = self.repo('one/nested')
        personal_exclusions = [self.repo(name) for name in ('Projects/submission', "Darren's 42 Repo/friend", 'mac-remote')]
        self.assertEqual(gobreak.select_repositories({'repo': str(one)})[0], [one])
        os.environ['GOBREAK_REPOS_ROOT'] = str(self.docs)
        os.environ['GOBREAK_EXCLUDE_REPOS'] = os.pathsep.join(map(str, [two, *personal_exclusions]))
        os.environ['GOBREAK_INCLUDE_REPOS'] = str(two)
        self.assertEqual(gobreak.select_repositories({})[0], [nested, one])
        os.environ['DOCUMENTS_PUSH_EXCLUDE'] = str(one)
        self.assertEqual(gobreak.select_repositories({})[0], [])

    def test_friends_have_no_folder_name_exclusions(self):
        for name in ("Darren's 42 Repo", 'Projects', 'mac-remote'):
            repo = self.repo(name)
            self.assertEqual(gobreak.select_repositories({'repo': str(repo)})[0], [repo])
        self.assertEqual(len(gobreak.select_repositories({'scan_root': str(self.docs)})[0]), 3)

    def test_real_checkpoint_and_lock_after_failure(self):
        repo = self.repo('space repo')
        remote = self.root / 'remote.git'
        subprocess.run(['git', 'init', '--bare', str(remote)], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        git(repo, 'remote', 'add', 'origin', str(remote))
        git(repo, 'push', '-u', 'origin', 'HEAD')
        (repo / 'work').write_text('changed\n')
        gobreak.checkpoint(repo)
        self.assertTrue(git(repo, 'log', '-1', '--format=%s').startswith('WIP: break checkpoint '))
        self.assertEqual(git(repo, 'log', '-1', '--format=%b'), '')
        self.assertEqual(git(repo, 'rev-parse', 'HEAD'), git(repo, 'rev-parse', '@{upstream}'))
        with patch.object(gobreak, 'wip_body', return_value='Pending implementation changes.'):
            (repo / 'work').write_text('changed again\n')
            gobreak.checkpoint(repo)
        self.assertEqual(git(repo, 'log', '-1', '--format=%b'), 'Pending implementation changes.')
        (repo / '.env').write_text('TEST_ONLY=example\n')
        marker = self.root / 'locked'
        (self.bin / 'ft_lock').write_text('#!/bin/sh\ntouch ' + str(marker) + '\n')
        with patch.object(sys, 'argv', ['gobreak.py', '--repo', str(repo)]):
            self.assertEqual(gobreak.main(), 1)
        self.assertTrue(marker.exists())

    def test_shell_and_standalone_updates_preserve_selection(self):
        repo = self.repo('selected repo')
        other = self.repo('other')
        runtime = self.root / 'shell runtime'
        target = self.root / '.zshrc'
        target.write_text('# Personal content\n')
        with contextlib.redirect_stdout(io.StringIO()):
            sync_zshrc.sync(SOURCE / 'Useful .zshrc edits (addition)', target, runtime=runtime)
            config = self.root / 'data/42-gobreak/installation.json'
            self.assertEqual(json.loads(config.read_text())['mode'], 'shell')
            install_gobreak.install(SOURCE, repo=repo, include_repos=[other], exclude_repos=[other])
            saved = config.read_text()
            (runtime / 'gobreak.py').write_text('# stale shell\n')
            standalone = self.root / 'data/42-gobreak/gobreak.py'
            standalone.write_text('# stale standalone\n')
            # Unchanged zshrc must still refresh both payloads.
            sync_zshrc.sync(SOURCE / 'Useful .zshrc edits (addition)', target, runtime=runtime)
            self.assertEqual(config.read_text(), saved)
            self.assertEqual(standalone.read_bytes(), (SOURCE / 'gobreak.py').read_bytes())
            self.assertEqual((runtime / 'gobreak.py').read_bytes(), standalone.read_bytes())
            (runtime / 'gobreak.py').write_text('# stale shell again\n')
            install_gobreak.install(SOURCE, repo=repo)
            self.assertEqual((runtime / 'gobreak.py').read_bytes(), standalone.read_bytes())
            before = {p: p.read_bytes() for p in self.root.rglob('*') if p.is_file()}
            sync_zshrc.sync(SOURCE / 'Useful .zshrc edits (addition)', target, check=True, runtime=runtime)
            self.assertEqual(before, {p: p.read_bytes() for p in self.root.rglob('*') if p.is_file()})
        self.assertTrue(target.read_text().startswith('# Personal content\n'))
        for launcher in (self.root / 'data/42-gobreak').glob('launch*'):
            subprocess.run(['bash', '-n', str(launcher)], check=True)

    def test_full_scope_uses_same_selection_and_locks(self):
        repo = self.repo('one')
        os.environ['GOBREAK_REPOS_ROOT'] = str(self.docs)
        with patch.object(sys, 'argv', ['gobreak.py', '--all-repos']), \
             patch.object(gobreak, 'break_checkpoint', return_value=1) as checkpoint, \
             patch.object(gobreak.subprocess, 'run', wraps=subprocess.run) as lock:
            self.assertEqual(gobreak.main(), 1)
            self.assertEqual(checkpoint.call_args.kwargs['repos'], [repo])
            self.assertEqual(lock.call_args.args, (['ft_lock'],))
            self.assertEqual(lock.call_args.kwargs, {'check': True})

    def test_full_wip_uses_shared_checkpoint(self):
        repo = self.repo('one')
        os.environ['MAIN_REPO_ROOT'] = str(repo)
        with patch.object(gobreak, 'checkpoint') as checkpoint:
            leaveschool.push(repo, wip=True)
            checkpoint.assert_called_once_with(repo, project_scope=True, codex_context=True)

    def test_pin_preserves_order_and_does_not_duplicate(self):
        with patch.object(install_gobreak.subprocess, 'run') as run:
            run.side_effect = [subprocess.CompletedProcess([], 0, "['terminal.desktop', 'browser.desktop']"),
                               subprocess.CompletedProcess([], 0, 'true'),
                               subprocess.CompletedProcess([], 0)]
            self.assertTrue(install_gobreak.pin_to_dock())
            favorites = run.call_args.args[0][-1]
            self.assertEqual(favorites, "['terminal.desktop', 'browser.desktop', 'local.42-gobreak.desktop']")
            run.reset_mock()
            run.side_effect = [subprocess.CompletedProcess([], 0, favorites)]
            self.assertTrue(install_gobreak.pin_to_dock())
            self.assertEqual(run.call_count, 1)

    def test_no_pin_survives_automatic_sync(self):
        repo = self.repo('one')
        with patch.object(install_gobreak, 'pin_to_dock') as pin:
            install_gobreak.install(SOURCE, repo=repo, pin=False)
            install_gobreak.install(SOURCE, automatic=True)
            pin.assert_not_called()
        config = self.root / 'data/42-gobreak/installation.json'
        self.assertFalse(json.loads(config.read_text())['pin_to_dock'])

    def test_empty_favorites_and_unavailable_settings(self):
        with patch.object(install_gobreak.subprocess, 'run') as run:
            run.side_effect = [subprocess.CompletedProcess([], 0, '@as []'),
                               subprocess.CompletedProcess([], 0, 'true'),
                               subprocess.CompletedProcess([], 0)]
            self.assertTrue(install_gobreak.pin_to_dock())
            self.assertEqual(run.call_args.args[0][-1], "['local.42-gobreak.desktop']")
            run.side_effect = subprocess.CalledProcessError(1, 'gsettings')
            self.assertFalse(install_gobreak.pin_to_dock())

    def test_no_codex_persists_and_prevents_cli_lookup(self):
        repo = self.repo('one')
        with contextlib.redirect_stdout(io.StringIO()):
            install_gobreak.install(SOURCE, repo=repo, codex_context=False, pin=False)
            install_gobreak.install(SOURCE, automatic=True)
        config = self.root / 'data/42-gobreak/installation.json'
        self.assertFalse(json.loads(config.read_text())['codex_context'])
        os.environ['GOBREAK_CODEX_CONTEXT'] = 'TRUE'
        def checkpoint(_):
            self.assertEqual(os.environ['GOBREAK_CODEX_CONTEXT'], 'FALSE')
            with patch.object(leaveschool.shutil, 'which') as lookup:
                self.assertEqual(leaveschool.wip_body(repo), '')
                lookup.assert_not_called()
        with patch.object(sys, 'argv', ['gobreak.py', '--repo', str(repo)]), \
             patch.object(gobreak, 'checkpoint', side_effect=checkpoint):
            self.assertEqual(gobreak.main(), 0)
        self.assertEqual(os.environ['GOBREAK_CODEX_CONTEXT'], 'TRUE')

    def test_missing_codex_prints_fallback_guidance(self):
        os.environ['GOBREAK_CODEX_CONTEXT'] = 'TRUE'
        output = io.StringIO()
        with patch.object(leaveschool.shutil, 'which', return_value=None), contextlib.redirect_stdout(output):
            self.assertEqual(leaveschool.wip_body(self.docs), '')
        self.assertIn('Codex CLI not found', output.getvalue())
        self.assertIn('--no-codex', output.getvalue())


if __name__ == '__main__':
    unittest.main()
