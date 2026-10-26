"""Shutdown tests use fake processes and never close a real browser."""
import signal
import os
import shutil
import subprocess
import tempfile
from pathlib import Path
import unittest
from unittest.mock import patch
import close_chrome as chrome

class CloseChrome(unittest.TestCase):
    def test_only_browser_parent_is_selected(self):
        with tempfile.TemporaryDirectory() as tmp:
            proc = Path(tmp)
            for pid, executable, args in ((11, 'chrome', b'chrome\0'),
                                          (12, 'chrome', b'chrome\0--type=renderer\0'),
                                          (13, 'chrome_crashpad_handler', b'chrome_crashpad_handler\0'),
                                          (14, 'chrome', b'chrome --type=zygote --flag\0')):
                directory = proc / str(pid)
                directory.mkdir()
                (directory/'exe').symlink_to('/opt/' + executable)
                (directory/'cmdline').write_bytes(args)
                (directory/'stat').write_text(str(pid) + ' (chrome) S ' + '0 ' * 18 + '1234 0')
            self.assertEqual(chrome.browsers(proc), {11: '1234'})
            self.assertFalse(chrome.still_running(11, 'different-start-time', proc))

    def test_shutdown_signals_parent_once_and_waits(self):
        with patch.object(chrome.sys, 'platform', 'linux'), patch.object(chrome, 'browsers', return_value={11: 'token'}), \
             patch.object(chrome, 'still_running', side_effect=[True, True, False]), \
             patch.object(chrome.os, 'kill') as kill, patch.object(chrome.time, 'sleep'):
            chrome.close()
            kill.assert_called_once_with(11, signal.SIGHUP)

    def test_timeout_does_not_force_kill(self):
        with patch.object(chrome.sys, 'platform', 'linux'), patch.object(chrome, 'browsers', return_value={11: 'token'}), \
             patch.object(chrome, 'still_running', return_value=True), patch.object(chrome.os, 'kill') as kill, \
             patch.object(chrome.time, 'monotonic', side_effect=[0, 31]):
            with self.assertRaisesRegex(RuntimeError, 'not force-killed'):
                chrome.close()
            kill.assert_called_once_with(11, signal.SIGHUP)

    @unittest.skipUnless(shutil.which('zsh'), 'zsh unavailable')
    def test_leavesch_keeps_tmux_on_chrome_timeout_and_still_locks(self):
        for failure in ('0', '1'):
            with self.subTest(failure=failure), tempfile.TemporaryDirectory() as tmp:
                root = Path(tmp)
                python = root/'python3'
                python.write_text('#!/bin/sh\necho "python:$*" >> "$EVIDENCE"\ncase "$1" in */close_chrome.py) exit "$CHROME_FAILURE";; esac\n')
                python.chmod(0o700)
                tmux = root/'tmux'
                tmux.write_text('#!/bin/sh\necho "tmux:$*" >> "$EVIDENCE"\n')
                tmux.chmod(0o700)
                env = dict(os.environ, EVIDENCE=str(root/'events'), CHROME_FAILURE=failure,
                           ZSHRC_SETTINGS_FILE=str(root/'missing'), ZSHRC_PORTABLE_SETTINGS_FILE='',
                           ZSHRC_RUNTIME_ROOT=tmp, DOCUMENTS_REPOS_ROOT=tmp, CODEX_PRIVATE_REPO_ROOT=tmp,
                           closeappsonleave='FALSE')
                script = r'''source "$1"
export PATH="$2:$PATH"
ft_lock() { print LOCK >> "$EVIDENCE"; }
leavesch
'''
                result = subprocess.run(['zsh', '-f', '-c', script, 'test',
                                         str(Path(__file__).with_name('Useful .zshrc edits (addition)')), tmp],
                                        env=env, capture_output=True, text=True)
                self.assertEqual(result.returncode, int(failure), result.stderr)
                events = (root/'events').read_text()
                self.assertIn('close_chrome.py', events)
                self.assertEqual('tmux:kill-server' in events, failure == '0')
                self.assertTrue(events.rstrip().endswith('LOCK'))

    def test_mac_uses_guarded_quit_and_waits(self):
        with patch.object(chrome.sys, 'platform', 'darwin'), patch.object(chrome.subprocess, 'run') as run:
            chrome.close()
            command = run.call_args.args[0]
            self.assertIn('if application "Google Chrome" is running', command[2])
            self.assertIn('repeat while', command[2])
            self.assertNotIn('System Events', command[2])

if __name__ == '__main__':
    unittest.main()
