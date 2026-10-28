import importlib.util
import os
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch

spec = importlib.util.spec_from_file_location('codex_update', Path(__file__).resolve().parents[1] / 'tools/codex_update.py')
updater = importlib.util.module_from_spec(spec)
spec.loader.exec_module(updater)


class UpdateTests(unittest.TestCase):
    def test_both_packages_then_daily_skip(self):
        with tempfile.TemporaryDirectory() as directory, patch.dict(os.environ, {'CODEX_HOME': directory}), patch.object(updater.subprocess, 'run') as run:
            updater.update()
            self.assertEqual(run.call_count, 3)
            installs = run.call_args_list[1:]
            self.assertEqual([c.kwargs['env']['CODEX_INSTALL_DAEMON_ONLY'] for c in installs], ['0', '1'])
            self.assertTrue(all(c.kwargs['env']['CODEX_RELEASE'] == 'latest' for c in installs))
            self.assertTrue(all(c.args[0][0] == 'sh' for c in installs))
            updater.update()
            self.assertEqual(run.call_count, 3)
            updater.update(force=True)
            self.assertEqual(run.call_count, 6)

    def test_failure_does_not_record_success(self):
        with tempfile.TemporaryDirectory() as directory, patch.dict(os.environ, {'CODEX_HOME': directory}), patch.object(updater.subprocess, 'run', side_effect=subprocess.CalledProcessError(1, 'curl')):
            with self.assertRaises(subprocess.CalledProcessError):
                updater.update()
            self.assertFalse((Path(directory) / 'daily-update/success').exists())


if __name__ == '__main__':
    unittest.main()
