"""Fake desktop backends only: never quit apps, push repos, or lock the desktop."""
import os
import subprocess
import tempfile
from pathlib import Path
import unittest
from unittest.mock import Mock, patch
import soft_quit_apps as quitapps

class SoftQuit(unittest.TestCase):
    def setUp(self):
        browser_patch = patch('close_chrome.browsers', return_value={})
        browser_patch.start(); self.addCleanup(browser_patch.stop)

    def test_system_terminal_and_personal_exclusions(self):
        apps = [quitapps.App(1, 'code'), quitapps.App(2, 'Gnome-terminal'),
                quitapps.App(3, 'gjs'), quitapps.App(4, 'slack'), quitapps.App(5, 'Google-chrome')]
        with patch.dict(os.environ, {'LEAVESCH_KEEP_APPS': 'slack', 'closechromeonleave': 'FALSE'}):
            self.assertEqual([a.name for a in quitapps.selected(apps)], ['code'])
            self.assertEqual([a.name for a in quitapps.selected(apps, True)], ['Gnome-terminal'])

    def test_only_enabled_quit_menu_actions_are_selected(self):
        for label in ('_Quit', 'Quit Telegram', 'Exit'):
            self.assertEqual(quitapps.quit_menu_item((7, {'label': label}, [])), 7)
        for props in ({'label': 'Force Quit'}, {'label': 'Quit without saving'},
                      {'label': 'Discard changes'}, {'label': 'Quit', 'enabled': False}):
            self.assertIsNone(quitapps.quit_menu_item((7, props, [])))

    def test_check_never_requests_quit(self):
        app = quitapps.App(123, 'Test', windows=[10], quit_action=Mock(), token='token')
        backend = Mock()
        backend.apps.return_value = {123: app}
        with patch.object(quitapps, 'X11', return_value=backend), patch.object(quitapps, 'NativeActions'), \
             patch.dict(os.environ, {'XDG_SESSION_TYPE': 'x11', 'LEAVESCH_KEEP_APPS': ''}):
            quitapps.linux_quit(check=True)
        app.quit_action.assert_not_called()
        backend.close_window.assert_not_called()

    def test_all_apps_get_requests_even_if_one_refuses_then_timeout(self):
        refusal = quitapps.App(1, 'unsaved-editor', [10], token='one')
        other = quitapps.App(2, 'chat', [20], quit_action=Mock(), token='two')
        backend = Mock()
        backend.apps.return_value = {1: refusal, 2: other}
        backend.close_window.return_value = True
        backend.clients.return_value = [10]
        with patch.object(quitapps, 'X11', return_value=backend), patch.object(quitapps, 'NativeActions'), \
             patch.object(quitapps, 'process_token', side_effect=lambda pid: {1:'one', 2:'two'}[pid]), \
             patch.object(quitapps.time, 'monotonic', side_effect=[0, 31]), \
             patch.dict(os.environ, {'XDG_SESSION_TYPE': 'x11', 'LEAVESCH_KEEP_APPS': ''}):
            with self.assertRaisesRegex(RuntimeError, 'check its prompts'):
                quitapps.linux_quit()
        backend.close_window.assert_called_once_with(10)
        other.quit_action.assert_called_once()

    def test_chrome_window_disappearing_does_not_mean_browser_exited(self):
        app = quitapps.App(1, 'Google-chrome', [10], token='one')
        backend = Mock(); backend.apps.return_value = {1:app}
        backend.close_window.return_value = True; backend.clients.return_value = []
        with patch.object(quitapps, 'X11', return_value=backend), patch.object(quitapps, 'NativeActions'), \
             patch.object(quitapps, 'process_token', return_value='one'), \
             patch.object(quitapps.time, 'monotonic', side_effect=[0, 1, 31]), \
             patch.object(quitapps.time, 'sleep'), \
             patch.dict(os.environ, {'XDG_SESSION_TYPE':'x11', 'LEAVESCH_KEEP_APPS':'', 'closechromeonleave':'TRUE'}):
            with self.assertRaisesRegex(RuntimeError, 'browser still running'):
                quitapps.linux_quit()
        backend.close_window.assert_called_once_with(10)

    def test_chrome_with_no_windows_is_not_silently_skipped(self):
        backend = Mock(); backend.apps.return_value = {}; backend.clients.return_value = []
        with patch.object(quitapps, 'X11', return_value=backend), patch.object(quitapps, 'NativeActions'), \
             patch('close_chrome.browsers', return_value={1:'one'}), \
             patch.object(quitapps, 'process_token', return_value='one'), \
             patch.object(quitapps.time, 'monotonic', side_effect=[0, 31]), \
             patch.dict(os.environ, {'XDG_SESSION_TYPE':'x11', 'LEAVESCH_KEEP_APPS':'', 'closechromeonleave':'TRUE'}):
            with self.assertRaisesRegex(RuntimeError, 'browser still running'):
                quitapps.linux_quit()
        backend.close_window.assert_not_called()

    def test_chrome_waits_until_process_exit_then_succeeds(self):
        app = quitapps.App(1, 'Google-chrome', [10], token='one')
        backend = Mock(); backend.apps.return_value = {1:app}
        backend.close_window.return_value = True; backend.clients.return_value = []
        with patch.object(quitapps, 'X11', return_value=backend), patch.object(quitapps, 'NativeActions'), \
             patch.object(quitapps, 'process_token', side_effect=['one','one','']), \
             patch.object(quitapps.time, 'monotonic', side_effect=[0, 1, 2]), \
             patch.object(quitapps.time, 'sleep'), \
             patch.dict(os.environ, {'XDG_SESSION_TYPE':'x11', 'LEAVESCH_KEEP_APPS':'', 'closechromeonleave':'TRUE'}):
            quitapps.linux_quit()

    def test_mac_requests_normal_quit_not_force_or_discard(self):
        report = Mock(stdout='{"planned":["Editor"],"pending":["Editor"]}')
        with patch.object(quitapps.subprocess, 'run', return_value=report) as run:
            with self.assertRaisesRegex(RuntimeError, 'Still open'):
                quitapps.mac_quit()
        script = run.call_args.args[0][4]
        self.assertIn('app.terminate;', script)
        self.assertNotIn('forceTerminate', script)
        self.assertNotIn('saving no', script)

    def test_leavesch_app_failure_retains_tmux_and_terminal_stage_is_last(self):
        for failure in ('0', '1', 'backup', 'push'):
            with self.subTest(failure=failure), tempfile.TemporaryDirectory() as tmp:
                root = Path(tmp)
                python = root/'python3'
                python.write_text('#!/bin/sh\necho "python:$*" >> "$EVIDENCE"\ncase "$APP_FAILURE:$1:$2" in 1:*/soft_quit_apps.py:|backup:*:backup|push:*:push-all) exit 1;; esac\n')
                python.chmod(0o700)
                tmux = root/'tmux'
                tmux.write_text('#!/bin/sh\necho "tmux:$*" >> "$EVIDENCE"\n')
                tmux.chmod(0o700)
                env = dict(os.environ, EVIDENCE=str(root/'events'), APP_FAILURE=failure,
                           ZSHRC_SETTINGS_FILE=str(root/'missing'), ZSHRC_PORTABLE_SETTINGS_FILE='',
                           ZSHRC_RUNTIME_ROOT=tmp, DOCUMENTS_REPOS_ROOT=tmp, CODEX_PRIVATE_REPO_ROOT=tmp,
                           closeappsonleave='TRUE')
                script = 'source "$1"\nexport PATH="$2:$PATH"\n_42_lock_screen() { print LOCK >> "$EVIDENCE"; }\nleavesch'
                result = subprocess.run(['zsh', '-f', '-c', script, 'test',
                                         str((Path(__file__).resolve().parents[1]/'Useful .zshrc edits (addition)')), tmp],
                                        env=env, capture_output=True, text=True)
                self.assertEqual(result.returncode, int(failure != '0'), result.stderr)
                events = (root/'events').read_text()
                self.assertEqual('tmux:kill-server' in events, failure == '0')
                self.assertEqual('--schedule-terminals' in events, failure == '0')
                self.assertTrue(events.rstrip().endswith('LOCK'))
                self.assertEqual(events.count('leaveschool.py backup'), 2)
                self.assertEqual(events.count('leaveschool.py push-all'), 2)
                self.assertIn('soft_quit_apps.py', events)
                if failure == '0':
                    self.assertLess(events.index('tmux:kill-server'), events.index('--schedule-terminals'))
                    self.assertEqual(events.count('leaveschool.py push-all'), 2)
                    self.assertLess(events.index('soft_quit_apps.py'), events.rindex('leaveschool.py push-all'))

if __name__ == '__main__':
    unittest.main()
