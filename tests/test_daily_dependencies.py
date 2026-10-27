"""First-run dependency recovery without touching real package managers."""
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / 'Useful .zshrc edits (addition)').read_text()
PREFLIGHT = SOURCE[SOURCE.index('_dailylogin_dependencies() {'):SOURCE.index('# Capture output before attaching')]

@unittest.skipUnless(shutil.which('zsh'), 'zsh unavailable')
class Dependencies(unittest.TestCase):
    def run_case(self, platform, manager, fail=False):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            # Every command is fake, including the package manager and sudo.
            stub = root / 'stub'
            stub.write_text('#!/bin/sh\nexit 0\n')
            stub.chmod(0o755)
            for name in ('git', 'python3', 'curl', 'make'):
                (root / name).symlink_to(stub)
            if manager:
                installer = root / manager
                installer.write_text('#!/bin/sh\nprintf "%s\\n" "$*" >> "$CASE_ROOT/log"\n' +
                    ('exit 9\n' if fail else
                     'case "$*" in *install*) /bin/ln -s "$CASE_ROOT/stub" "$CASE_ROOT/tmux" ;; esac\n'))
                installer.chmod(0o755)
            sudo = root / 'sudo'
            sudo.write_text('#!/bin/sh\n[ "$1" != -n ] || shift\nexec "$@"\n')
            sudo.chmod(0o755)
            result = subprocess.run([shutil.which('zsh'), '-f', '-c',
                PREFLIGHT + '\n_dailylogin_enabled() { return 1; }\nOSTYPE=' + platform +
                '\n_dailylogin_dependencies'], env={**os.environ, 'PATH': tmp, 'CASE_ROOT': tmp},
                text=True, capture_output=True)
            log = (root / 'log').read_text() if (root / 'log').exists() else ''
            return result, log

    def test_ubuntu_installs_then_verifies(self):
        result, log = self.run_case('linux-gnu', 'apt-get')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(log.splitlines(), ['update', 'install -y tmux'])

    def test_mac_installs_then_verifies(self):
        result, log = self.run_case('darwin', 'brew')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(log.strip(), 'install tmux')

    def test_install_failure_reports_recovery(self):
        result, _ = self.run_case('linux-gnu', 'apt-get', fail=True)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('sudo apt-get update', result.stderr)
        self.assertIn('rerun dailylogin', result.stderr)

    def test_missing_homebrew_explains_bootstrap(self):
        result, _ = self.run_case('darwin', None)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('https://brew.sh', result.stderr)
