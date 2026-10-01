#!/usr/bin/env python3
"""Regression checks for profiling, summary statistics and resumable records."""
import importlib.util
from pathlib import Path
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('runner', ROOT / 'tests/run_random_tests.py')
runner = importlib.util.module_from_spec(spec)
spec.loader.exec_module(runner)


class ProfileTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        import subprocess
        subprocess.run(['make', '-s'], cwd=ROOT, check=True)
        cls.folder = tempfile.TemporaryDirectory(prefix='push_swap_profile_test_')
        cls.binary = Path(cls.folder.name) / 'solver'
        runner.profile_build(cls.binary)

    @classmethod
    def tearDownClass(cls):
        cls.folder.cleanup()

    def test_actual_candidates_and_round_trip(self):
        for values, expected in [([0, 1, 2], 0), ([2, 1, 0], 6),
                                 (list(range(10, -1, -1)), 5)]:
            with self.subTest(values=values):
                result = runner.run_case(values, executable=self.binary)
                self.assertEqual(result['checker'], 'OK')
                self.assertEqual(len(result['algorithms']), expected)
                for algorithm in result['algorithms']:
                    self.assertEqual(sum(algorithm['counts'].values()), algorithm['moves'])
                    self.assertGreaterEqual(algorithm['seconds'], 0)
                    self.assertGreaterEqual(algorithm['cpu_seconds'], 0)
                    self.assertGreater(algorithm['process_peak_rss_kib'], 0)
                record = dict(result, run_id=1, generation_id=0, seed=42,
                              rank_sha256=runner.rank_hash(values), input_ranks=values,
                              binary_sha256='test', settings={'DEBUG': 0})
                self.assertEqual(runner.parse_records(runner.render_record(record)), [record])
                config = dict(prefix='test', seed=42, size=len(values),
                              generator=runner.GENERATOR, python_version='test')
                path = Path(self.folder.name) / 'summary.md'
                runner.save_summary(path, config, [record], 'complete')
                self.assertIn('Per-run algorithm data', path.read_text())
                self.assertIn('Moves Sample SD', path.read_text())

    def test_failed_candidate_event(self):
        event = runner.parse_profile('0\tBFS\t1\t0.1\t0.1\t1000\t\n')[0]
        self.assertEqual(event['status'], 1)
        self.assertEqual(event['moves'], 0)

    def test_statistics_and_old_markdown(self):
        self.assertEqual(runner.distribution([1, 3])[0], 2)
        self.assertEqual(runner.distribution([1])[1], '—')
        self.assertEqual(runner.quantile([0, 10], .95), 9.5)
        record = dict(run_id=1, generation_id=0, seed=42, rank_sha256='test',
                      binary_sha256='test', checker='OK', move_count=1,
                      solver_seconds=.01, solver_exit=0, settings={}, input_ranks=[1, 0],
                      moves=['sa'], solution_debug='', completed_status=[],
                      algorithms=[dict(name='legacy', moves=1)])
        text = runner.render_record(record)
        section = runner.code_section('Algorithm metrics JSON',
                                      __import__('json').dumps(record['algorithms'], sort_keys=True))
        self.assertEqual(runner.parse_records(text.replace(section, '')), [record])


if __name__ == '__main__':
    unittest.main()
