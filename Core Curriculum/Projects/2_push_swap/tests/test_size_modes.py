#!/usr/bin/env python3
"""Pure generator/summary tests: no solver launches or builds."""
import importlib.util
from pathlib import Path
import tempfile
import unittest

spec = importlib.util.spec_from_file_location('runner', Path(__file__).with_name('run_random_tests.py'))
runner = importlib.util.module_from_spec(spec)
spec.loader.exec_module(runner)


class SizeModes(unittest.TestCase):
    def test_loop_and_random(self):
        config = dict(seed=42, size_mode='loop', min_size=2, max_size=600)
        self.assertEqual([runner.trial_size(config, n) for n in [0, 1, 598, 599, 600]],
                         [2, 3, 600, 2, 3])
        config['size_mode'] = 'random'
        first = [runner.trial_size(config, n) for n in range(1000)]
        self.assertEqual(first, [runner.trial_size(config, n) for n in range(1000)])
        self.assertTrue(all(2 <= n <= 600 for n in first))
        self.assertGreater(len(set(first)), 100)
        for generation in [0, 17, 999]:
            size = runner.trial_size(config, generation)
            values = runner.generate(42, generation, size)
            self.assertEqual(sorted(values), list(range(size)))
            self.assertEqual(values, runner.generate(42, generation, size))

    def test_resume_config_and_size_groups(self):
        config = dict(prefix='test', seed=42, size=100, size_mode='loop',
                      min_size=2, max_size=600, generator=runner.GENERATOR, python_version='test')
        records = []
        for run_id, size in enumerate([2, 3, 2], 1):
            values = list(range(size))
            records.append(dict(run_id=run_id, generation_id=run_id-1, seed=42,
                                input_ranks=values, rank_sha256=runner.rank_hash(values),
                                binary_sha256='test', settings={}, move_count=size,
                                solver_seconds=.1, algorithms=[dict(id=1, name='test', moves=size,
                                                                    seconds=.05, counts={'ra': size})]))
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder)/'summary.md'
            runner.save_summary(path, config, records, 'complete')
            recovered = runner.read_config(path)
            self.assertEqual(recovered['size_mode'], 'loop')
            self.assertEqual(recovered['max_size'], 600)
            self.assertEqual(runner.trial_size(recovered, 599), 2)
            comparison = path.read_text().split('## Algorithm comparison')[1].split('## Per-run')[0]
            self.assertIn('| test | {} | 2 | test | 2 |', comparison)
            self.assertIn('| test | {} | 3 | test | 1 |', comparison)
        self.assertEqual(runner.trial_size(dict(size=100), 999), 100)


if __name__ == '__main__':
    unittest.main()
