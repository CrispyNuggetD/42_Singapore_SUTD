"""Isolated tmux and desktop command regression checks; never touch user sessions."""
import importlib.util
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest
from unittest.mock import patch, Mock

spec = importlib.util.spec_from_file_location('daily_terminal', Path(__file__).with_name('daily_terminal.py'))
daily = importlib.util.module_from_spec(spec)
spec.loader.exec_module(daily)

class Desktop(unittest.TestCase):
    def test_routing_and_maximize_are_personalizable(self):
        with patch.dict(os.environ, {'DAILY_TMUX_SESSION': 'work', 'LAN42_TMUX_SESSION': 'campus'}, clear=True):
            self.assertEqual(daily.session_name('codex'), 'work')
            self.assertEqual(daily.session_name('lan42-game-123'), 'campus')
            self.assertTrue(daily.maximize_enabled())
            os.environ['DAILY_TMUX_MAXIMIZE'] = 'FALSE'
            self.assertFalse(daily.maximize_enabled())

    def test_desktop_launch_is_maximized_without_fullscreen_on_linux_and_mac(self):
        for platform in ('linux', 'darwin'):
            for maximize in ('TRUE', 'FALSE'):
                with self.subTest(platform=platform, maximize=maximize), tempfile.TemporaryDirectory() as tmp:
                    environment = {'XDG_STATE_HOME': tmp, 'DISPLAY': ':test', 'DAILY_TMUX_MAXIMIZE': maximize}
                    with patch.dict(os.environ, environment, clear=True), patch.object(daily.sys, 'platform', platform), \
                         patch.object(daily, 'exists', return_value=True), \
                         patch.object(daily, 'attached', side_effect=[0, 1]), \
                         patch.object(daily.shutil, 'which', return_value='/usr/bin/gnome-terminal'), \
                         patch.object(daily.subprocess, 'Popen', return_value=Mock()) as launch:
                        daily.show('daily')
                        command = launch.call_args.args[0]
                        if platform == 'linux':
                            self.assertEqual('--maximize' in command, maximize == 'TRUE')
                            self.assertNotIn('--full-screen', command)
                        else:
                            self.assertEqual('set zoomed' in command[2], maximize == 'TRUE')

@unittest.skipUnless(shutil.which('zsh'), 'zsh unavailable')
class Shell(unittest.TestCase):
    def test_portable_project_refresh_and_login_script_are_isolated(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root/'Projects'/'new-project').mkdir(parents=True)
            (root/'portable.zsh').write_text('export current_proj=new-project\n')
            (root/'personal.zsh').write_text('export current_proj=old-project\nexport ZSHRC_SETUP_DONE=1\n')
            script = r'''
export ZSHRC_SETTINGS_FILE="$1/personal.zsh"
export ZSHRC_PORTABLE_SETTINGS_FILE="$1/portable.zsh"
export MAIN_REPO_ROOT="$1" PROJECTS_ROOT=Projects XDG_STATE_HOME="$1/state"
source "$2"
cdcurproj || exit 2
[[ "$PWD" == "$1/Projects/new-project" ]] || exit 3
_dailylogin_impl() { print DAILY_OUTPUT; return 7; }
_privatebackup_check() { :; }
_login_items_ensure() { :; }
_daily_terminal() { print -r -- "HELPER:$*" >> "$evidence"; }
evidence="$1/evidence"
LOGIN_ITEMS_SCRIPT="$1/my script.sh"
LOGIN_ITEMS_TMUX_ROLE=my-startup
loginitems=TRUE
dli 'one argument'
[[ $? == 7 ]] || exit 4
# Process-substitution tee can finish just after the function returns.
wait
print -r -- "PROJECT:$current_proj"
'''
            result = subprocess.run(['zsh', '-f', '-c', script, 'test', tmp,
                                     str(Path(__file__).with_name('Useful .zshrc edits (addition)'))],
                                    text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stderr + result.stdout)
            self.assertIn('PROJECT:new-project', result.stdout)
            self.assertIn('DAILY_OUTPUT', (root/'state/42-shell/dailylogin.log').read_text())
            evidence = (root/'evidence').read_text().splitlines()
            self.assertIn('dailylogin -- tail', evidence[0])
            self.assertIn('my-startup -- bash ' + tmp + '/my script.sh one argument', evidence[1])
            self.assertEqual(evidence[2], 'HELPER:show-all')

@unittest.skipUnless(shutil.which('tmux'), 'tmux unavailable')
class Sessions(unittest.TestCase):
    def test_legacy_migration_preserves_running_processes(self):
        with tempfile.TemporaryDirectory() as tmp, patch.dict(os.environ, {
                'DAILY_TMUX_SOCKET': tmp + '/tmux.sock', 'XDG_STATE_HOME': tmp,
                'DAILY_TMUX_SESSION': 'work', 'LAN42_TMUX_SESSION': 'campus'}):
            try:
                lobby = daily.open_window('work', 'lan42', tmp, ['sleep', '60'])
                shell = daily.open_window('work', 'zsh', tmp, ['sleep', '60'])
                pid = daily.tmux('display-message', '-p', '-t', lobby, '#{pane_pid}').stdout
                daily.migrate_layout()
                self.assertEqual(daily.find_window('campus', 'lan42'), lobby)
                self.assertEqual(daily.find_window('work', 'current-project'), shell)
                self.assertEqual(daily.tmux('display-message', '-p', '-t', lobby, '#{pane_pid}').stdout, pid)
            finally:
                daily.tmux('kill-server', check=False)

    def test_roles_reuse_windows_and_keep_lan_separate(self):
        with tempfile.TemporaryDirectory() as tmp, patch.dict(os.environ, {
                'DAILY_TMUX_SOCKET': tmp + '/tmux.sock', 'XDG_STATE_HOME': tmp,
                'DAILY_TMUX_SESSION': 'work', 'LAN42_TMUX_SESSION': 'campus'}):
            try:
                work = daily.open_window(daily.session_name('current-project'), 'current-project', tmp, ['sleep', '60'])
                again = daily.open_window('work', 'current-project', tmp, ['sleep', '60'])
                self.assertEqual(work, again)
                daily.open_window(daily.session_name('lan42'), 'lan42', tmp, ['sleep', '60'])
                daily.open_window(daily.session_name('lan42-game-1'), 'lan42-game-1', tmp, ['sleep', '60'])
                self.assertEqual(daily.tmux('list-windows', '-t', '=work', '-F', '#W').stdout.strip(), 'Current Project')
                self.assertEqual(len(daily.tmux('list-windows', '-t', '=campus').stdout.splitlines()), 2)
            finally:
                daily.tmux('kill-server', check=False)

if __name__ == '__main__':
    unittest.main()
