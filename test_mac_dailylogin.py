"""Mac workflow checks with mocked OS requests; never quit apps or lock a screen."""
import fcntl
import os
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import Mock, patch
import app_manager
import create_login_items
import mac_mailbox

ROOT = Path(__file__).resolve().parent


class MacApps(unittest.TestCase):
    def test_app_names_and_bundle_paths_use_launchservices(self):
        with patch.object(app_manager.sys, 'platform', 'darwin'), patch.object(app_manager.subprocess, 'run', return_value=Mock(returncode=0)):
            self.assertEqual(app_manager.resolve(['Music']), ['/usr/bin/open', '-a', 'Music'])
            self.assertEqual(app_manager.resolve(['/Applications/Music.app']), ['/usr/bin/open', '-a', '/Applications/Music.app'])
            self.assertFalse(app_manager.already_running(['/usr/bin/open', '-a', 'Music']))
            self.assertEqual(app_manager.app_from_command('open -a Music')['name'], 'Music')

    def test_empty_script_prints_help_and_is_never_overwritten(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'login script.sh'
            self.assertTrue(create_login_items.ensure(path))
            result = subprocess.run(['bash', str(path)], capture_output=True, text=True, check=True)
            self.assertIn(str(path), result.stdout)
            self.assertIn('setupapps', result.stdout)
            path.write_text('echo PERSONAL\n')
            self.assertFalse(create_login_items.ensure(path))
            self.assertEqual(path.read_text(), 'echo PERSONAL\n')


class MacMailbox(unittest.TestCase):
    def test_live_lock_and_stale_file_are_distinguished(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            path = root / '.mailbox/mac/worker.lock'
            path.parent.mkdir(parents=True)
            self.assertFalse(mac_mailbox.running(root))
            with path.open('a') as lock:
                fcntl.flock(lock, fcntl.LOCK_EX)
                self.assertTrue(mac_mailbox.running(root))
            self.assertFalse(mac_mailbox.running(root))

    def test_active_service_is_kept_even_when_tmux_mode_requested(self):
        with patch.object(mac_mailbox, 'running', return_value=True), patch.object(mac_mailbox.subprocess, 'run') as run:
            argv = mac_mailbox.command(Path('/runtime'), Path('/source'), 'tmux')
            self.assertEqual(argv[:4], ['tail', '-n', '120', '-F'])
            run.assert_not_called()

    def test_foreground_starts_only_when_no_other_worker_holds_lock(self):
        with tempfile.TemporaryDirectory() as tmp:
            runtime = Path(tmp)
            (runtime / 'mac.sh').touch()
            with patch.object(mac_mailbox, 'running', return_value=False):
                self.assertEqual(mac_mailbox.command(runtime, runtime, 'tmux'),
                                 ['bash', str(runtime / 'mac.sh'), 'run', '--full-access'])


class MacShell(unittest.TestCase):
    def test_mac_brightness_hook_and_chat_export_use_local_configuration(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            fake = root / 'python3'
            fake.write_text('#!/bin/sh\nprintf "%s\\n" "$*" >> "$EVIDENCE"\n')
            fake.chmod(0o755)
            script = '''source "$1"
export PATH="$2:$PATH"
OSTYPE=darwin
adjustbrightness=TRUE
setbrightness=63
my_brightness() { print -r -- "BRIGHT:$1" >> "$EVIDENCE"; }
MAC_BRIGHTNESS_FUNCTION=my_brightness
adjust_brightness || exit 2
_leavesch_backup || exit 3
'''
            env = dict(os.environ, EVIDENCE=str(root / 'evidence'), ZSHRC_SETTINGS_FILE=str(root/'none'),
                       ZSHRC_PORTABLE_SETTINGS_FILE='', CODEX_PRIVATE_REPO_ROOT='/private/local',
                       ZSHRC_RUNTIME_ROOT='/runtime/local')
            result = subprocess.run(['zsh', '-f', '-c', script, 'test', str(ROOT/'Useful .zshrc edits (addition)'), tmp],
                                    env=env, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            rows = (root/'evidence').read_text().splitlines()
            self.assertEqual(rows[0], 'BRIGHT:63')
            self.assertEqual(rows[1], '/runtime/local/codex_return.py export --private /private/local')
            self.assertEqual(rows[2], '/runtime/local/leaveschool.py backup')


if __name__ == '__main__':
    unittest.main()
