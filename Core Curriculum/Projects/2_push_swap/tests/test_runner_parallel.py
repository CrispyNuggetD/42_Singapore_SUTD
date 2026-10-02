#!/usr/bin/env python3
"""Exercise scheduling and persistence with mocked solvers; no compilation or solver runs."""
import argparse
import contextlib
import importlib.util
import io
from pathlib import Path
import tempfile
import threading
import time
import unittest
from unittest.mock import patch

spec = importlib.util.spec_from_file_location('runner', Path(__file__).with_name('run_random_tests.py'))
runner = importlib.util.module_from_spec(spec)
spec.loader.exec_module(runner)


class ParallelTests(unittest.TestCase):
    def test_ram_and_light_limits(self):
        config = dict(size=500, size_mode='fixed')
        with patch.object(runner, 'physical_cpu_count', return_value=12):
            self.assertEqual(runner.worker_limit(config, {}, light=True)[0], 1)
            self.assertLessEqual(runner.worker_limit(config, {})[0], 11)
            self.assertLessEqual(runner.worker_limit(config, {}, requested=2)[0], 2)
        _, bfs_bytes = runner.worker_limit(dict(size=10), {'BRUTE_MAX_N': 10})
        self.assertGreater(bfs_bytes, 1024**3)

    def test_parallel_commit_and_resume(self):
        active = peak = 0
        guard = threading.Lock()

        def fake_case(values, *args):
            nonlocal active, peak
            with guard:
                active += 1
                peak = max(peak, active)
            time.sleep(.02)
            with guard:
                active -= 1
            return dict(solver_exit=0, solver_seconds=.02, move_count=1,
                        moves=['sa'], solution_debug='', algorithms=[],
                        completed_status=[], checker='OK')

        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            (root / 'includes').mkdir()
            (root / 'includes/push_swap.h').write_text('# define DEBUG 0\n')
            (root / 'checker_linux').touch()
            (root / 'checker_linux').chmod(0o700)
            options = argparse.Namespace(n=4, size=3, random=False, loop=False,
                        min_size=None, max_size=None, seed=42, show_solutions=False,
                        resume=None, resume_latest=False, jobs=2, light=False)
            with patch.object(runner, 'ROOT', root), \
                 patch.object(runner, 'arguments', return_value=options), \
                 patch.object(runner, 'worker_limit', return_value=(2, 128*1024**2)), \
                 patch.object(runner.subprocess, 'run'), \
                 patch.object(runner, 'profile_build', side_effect=lambda p: p.write_bytes(b'fake')), \
                 patch.object(runner, 'run_case', side_effect=fake_case), \
                 contextlib.redirect_stdout(io.StringIO()):
                self.assertEqual(runner.main(), 0)
                summary = next(root.rglob('*_output_summary.md'))
                config = runner.read_config(summary)
                saved = runner.load_records(summary.parent, config['prefix'])
                self.assertEqual([r['run_id'] for r in saved], [1, 2, 3, 4])
                self.assertEqual(len({r['rank_sha256'] for r in saved}), 4)
                self.assertTrue(all(r['settings']['BENCHMARK_WORKERS'] == 2 for r in saved))
                options.resume = summary
                options.n = 1
                self.assertEqual(runner.main(), 0)
                self.assertEqual(len(runner.load_records(summary.parent, config['prefix'])), 5)
            self.assertEqual(peak, 2)


if __name__ == '__main__':
    unittest.main()
