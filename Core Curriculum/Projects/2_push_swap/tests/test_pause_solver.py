import importlib.util
import os
from pathlib import Path
import signal
import subprocess
import sys
import tempfile
import time
import unittest

spec = importlib.util.spec_from_file_location('pause_solver', Path(__file__).with_name('pause_solver.py'))
pause = importlib.util.module_from_spec(spec)
spec.loader.exec_module(pause)


@unittest.skipUnless(sys.platform.startswith('linux'), 'Linux process control')
class PauseTests(unittest.TestCase):
    def test_runner_and_separate_session_child_pause_and_resume(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = root / 'run_random_tests.py'
            child_file = root / 'child'
            script.write_text('import subprocess,time,pathlib\n'
                              'p=subprocess.Popen(["sleep","60"],start_new_session=True)\n'
                              'pathlib.Path(' + repr(str(child_file)) + ').write_text(str(p.pid))\n'
                              'p.wait()\n')
            runner = subprocess.Popen([sys.executable, str(script)], start_new_session=True)
            child = None
            try:
                for _ in range(100):
                    if child_file.exists() and child_file.read_text():
                        child = int(child_file.read_text())
                        break
                    time.sleep(.01)
                self.assertIsNotNone(child)
                self.assertNotEqual(os.getpgid(child), os.getpgid(runner.pid))
                self.assertIn('paused', pause.toggle(runner.pid, root / 'state'))
                for pid in (runner.pid, child):
                    pause.await_stopped(pid)
                    self.assertEqual(Path('/proc', str(pid), 'stat').read_text().rsplit(')', 1)[1].split()[0], 'T')
                self.assertEqual(pause.toggle(runner.pid, root / 'state'), 'Solver resumed')
                self.assertEqual(list((root / 'state').glob('*.json')), [])
            finally:
                for pid in (runner.pid, child):
                    if pid:
                        try:
                            os.kill(pid, signal.SIGCONT)
                            os.kill(pid, signal.SIGTERM)
                        except ProcessLookupError:
                            pass
                runner.wait(timeout=3)

    def test_other_pane_is_rejected(self):
        with tempfile.TemporaryDirectory() as tmp:
            with self.assertRaisesRegex(ValueError, 'Select the pane'):
                pause.toggle(os.getpid(), Path(tmp))


if __name__ == '__main__':
    unittest.main()
