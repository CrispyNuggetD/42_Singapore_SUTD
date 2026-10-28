import importlib.util
import os
from pathlib import Path
import signal
import subprocess
import sys
import tempfile
import time
import unittest
from unittest.mock import patch

spec = importlib.util.spec_from_file_location('pause_solver', Path(__file__).with_name('pause_solver.py'))
pause = importlib.util.module_from_spec(spec)
spec.loader.exec_module(pause)


@unittest.skipUnless(sys.platform.startswith('linux'), 'Linux tmux helper')
class PauseTests(unittest.TestCase):
    def test_native_keyboard_and_tmux_requests_pause_separate_worker_group(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = root / 'run_random_tests.py'
            child_file = root / 'child'
            source = str(Path(__file__).with_name('run_random_tests.py'))
            script.write_text('import importlib.util,pathlib,subprocess\n'
                              's=importlib.util.spec_from_file_location("runner",' + repr(source) + ')\n'
                              'r=importlib.util.module_from_spec(s);s.loader.exec_module(r)\n'
                              'c=r.PauseControls();c.start()\n'
                              'p=r.start_process(["sleep","60"],start_new_session=True)\n'
                              'pathlib.Path(' + repr(str(child_file)) + ').write_text(str(p.pid))\n'
                              'try:p.wait()\n'
                              'finally:r.forget_process(p);c.close()\n')
            master, slave = os.openpty()
            runner = subprocess.Popen([sys.executable, str(script)], stdin=slave,
                                      stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                                      env=dict(os.environ, XDG_STATE_HOME=tmp), start_new_session=True)
            os.close(slave)
            child = None
            def state(pid):
                return Path('/proc', str(pid), 'stat').read_text().rsplit(')', 1)[1].split()[0]
            def await_state(stopped):
                for _ in range(100):
                    if (state(child) == 'T') == stopped:return
                    time.sleep(.01)
                self.fail('Worker did not change suspension state')
            try:
                for _ in range(100):
                    if child_file.exists() and child_file.read_text():
                        child = int(child_file.read_text());break
                    time.sleep(.01)
                self.assertIsNotNone(child)
                self.assertNotEqual(os.getpgid(child), os.getpgid(runner.pid))
                os.write(master, b'p')
                await_state(True)
                self.assertNotEqual(state(runner.pid), 'T')
                with patch.dict(os.environ, {'XDG_STATE_HOME': tmp}):
                    self.assertIn('requested', pause.toggle(runner.pid))
                await_state(False)
            finally:
                if child:
                    try:
                        os.kill(child, signal.SIGCONT);os.kill(child, signal.SIGTERM)
                    except ProcessLookupError:pass
                try:runner.wait(timeout=3)
                except subprocess.TimeoutExpired:
                    runner.kill();runner.wait()
                os.close(master)

    def test_other_pane_is_rejected(self):
        with self.assertRaisesRegex(ValueError, 'Select the pane'):
            pause.toggle(os.getpid())


if __name__ == '__main__':
    unittest.main()
