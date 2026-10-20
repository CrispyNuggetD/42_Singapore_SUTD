"""Bucket boundaries, legacy migration, and resumable report links."""
import importlib.util
import json
from pathlib import Path
import tempfile
import unittest

spec = importlib.util.spec_from_file_location('runner', Path(__file__).with_name('run_random_tests.py'))
runner = importlib.util.module_from_spec(spec)
spec.loader.exec_module(runner)


class LayoutTests(unittest.TestCase):
    def test_boundaries_and_unbounded_numbering(self):
        root = Path('/session')
        expected = {1: '00/00', 100: '00/00', 101: '00/01',
                    10000: '00/99', 10001: '01/00',
                    1000000: '99/99', 1000001: '01/00/00'}
        for number, bucket in expected.items():
            path = runner.detail_path(root, 'test', number)
            self.assertEqual(path.parent, root / 'details' / bucket)
        # Ten times today's data and far beyond: each leaf has <=100 reports.
        counts = {}
        for number in range(1, 20001):
            parent = runner.detail_path(root, 'test', number).parent
            counts[parent] = counts.get(parent, 0) + 1
        self.assertEqual(max(counts.values()), 100)

    def test_mixed_migration_roundtrip_and_links(self):
        with tempfile.TemporaryDirectory() as name:
            root = Path(name)
            config = dict(prefix='test', seed=42, size=20,
                          generator=runner.GENERATOR, python_version='test')
            records = []
            for i in range(21):
                values = runner.generate(42, i, 20)
                record = dict(run_id=i+1, generation_id=i, seed=42,
                              rank_sha256=runner.rank_hash(values), input_ranks=values,
                              binary_sha256='test', settings={}, checker='OK',
                              move_count=1, solver_seconds=.01, solver_exit=0,
                              moves=['sa'], solution_debug='', completed_status=[], algorithms=[])
                runner.save_detail(root, config, records, record)
                records.append(record)
            first = runner.detail_path(root, 'test', 1)
            legacy = root / first.name
            first.rename(legacy)
            runner.save_summary(root / 'test_output_summary.md', config, records, 'running')
            self.assertIn('](test_output_000001.md)', (root / 'test_output_summary.md').read_text())
            self.assertEqual(runner.load_records(root, 'test'), records)
            runner.migrate_details(root, 'test')
            runner.migrate_details(root, 'test')  # Safe after interruption/repeated resume.
            self.assertFalse(legacy.exists())
            self.assertEqual(runner.load_records(root, 'test'), records)
            runner.save_summary(root / 'test_output_summary.md', config, records, 'running')
            self.assertIn('](details/00/00/test_output_000001.md)',
                          (root / 'test_output_summary.md').read_text())
            # A partial final report remains appendable after migration.
            self.assertEqual(len(runner.parse_records(runner.detail_path(root, 'test', 3).read_text())), 1)

    def test_legacy_jsonl_and_numeric_order(self):
        with tempfile.TemporaryDirectory() as name:
            root = Path(name)
            record = dict(run_id=1, input_ranks=[1, 0],
                          rank_sha256=runner.rank_hash([1, 0]))
            legacy = root / 'test_output_000001.txt'
            legacy.write_text(json.dumps(record) + '\n')
            self.assertEqual(runner.load_records(root, 'test'), [record])
            runner.migrate_details(root, 'test')
            self.assertEqual(runner.load_records(root, 'test'), [record])
            self.assertTrue(runner.detail_path(root, 'test', 1, '.txt').exists())

    def test_duplicate_is_rejected_and_large_numbers_discovered(self):
        with tempfile.TemporaryDirectory() as name:
            root = Path(name)
            target = runner.detail_path(root, 'test', 1000001)
            target.parent.mkdir(parents=True)
            target.write_text('original')
            self.assertEqual(runner.detail_paths(root, 'test'), {1000001: target})
            legacy = root / target.name
            legacy.write_text('different')
            with self.assertRaises(ValueError):
                runner.migrate_details(root, 'test')
            self.assertEqual(target.read_text(), 'original')
            self.assertEqual(legacy.read_text(), 'different')


if __name__ == '__main__':
    unittest.main()
