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
        bfs_bytes = runner.memory_allowance(10, {'BRUTE_MAX_N': 10})
        self.assertGreater(bfs_bytes, 1024**3)

    def test_per_input_memory_admission(self):
        mib = 1024 ** 2
        with patch.object(runner, 'available_memory', return_value=8*1024**3):
            heavy = runner.memory_allowance(10, {'BRUTE_MAX_N': 10})
            light = runner.memory_allowance(11, {'BRUTE_MAX_N': 10})
            self.assertGreater(heavy, light)
            budget = 3 * heavy
            self.assertFalse(runner.can_schedule(10, {'BRUTE_MAX_N': 10},
                                                3*heavy, budget, 3))
            self.assertTrue(runner.can_schedule(11, {'BRUTE_MAX_N': 10},
                                               2*heavy, budget, 2))
            self.assertTrue(runner.can_schedule(11, {'BRUTE_MAX_N': 10},
                                               8*light, budget, 8))

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
                 patch.object(runner, 'worker_limit', return_value=(2, 4*1024**3)), \
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


    def test_worker_count_grows_after_heavy_inputs(self):
        active = peak = 0
        guard = threading.Lock()

        def fake_case(values, *args):
            nonlocal active, peak
            with guard:
                active += 1
                peak = max(peak, active)
            time.sleep(.07 if len(values) <= 10 else .025)
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
            options = argparse.Namespace(n=8, size=100, random=False, loop=True,
                        min_size=9, max_size=20, seed=42, show_solutions=False,
                        resume=None, resume_latest=False, jobs=4, light=False)
            with patch.object(runner, 'ROOT', root), \
                 patch.object(runner, 'arguments', return_value=options), \
                 patch.object(runner, 'worker_limit', return_value=(4, 20*1024**2)), \
                 patch.object(runner, 'memory_allowance', side_effect=lambda n, _: (10 if n <= 10 else 1)*1024**2), \
                 patch.object(runner, 'available_memory', return_value=100*1024**2), \
                 patch.object(runner.subprocess, 'run'), \
                 patch.object(runner, 'profile_build', side_effect=lambda p: p.write_bytes(b'fake')), \
                 patch.object(runner, 'run_case', side_effect=fake_case), \
                 contextlib.redirect_stdout(io.StringIO()):
                self.assertEqual(runner.main(), 0)
                summary = next(root.rglob('*_output_summary.md'))
                config = runner.read_config(summary)
                saved = runner.load_records(summary.parent, config['prefix'])
                self.assertEqual([r['run_id'] for r in saved], list(range(1, 9)))
                self.assertEqual(len({r['rank_sha256'] for r in saved}), 8)
                self.assertTrue(all(r['settings']['BENCHMARK_WORKERS'] == 4 for r in saved))
                options.resume = summary
                options.n = 1
                self.assertEqual(runner.main(), 0)
                self.assertEqual(len(runner.load_records(summary.parent, config['prefix'])), 9)
            self.assertEqual(peak, 4)


if __name__ == '__main__':
    unittest.main()
