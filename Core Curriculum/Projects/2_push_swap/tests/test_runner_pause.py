import importlib.util
import io
from pathlib import Path
import tempfile
import threading
import time
import unittest
from unittest.mock import Mock, patch

spec = importlib.util.spec_from_file_location('runner_pause', Path(__file__).with_name('run_random_tests.py'))
runner = importlib.util.module_from_spec(spec)
spec.loader.exec_module(runner)


def record(number=1, paused=False):
    result = dict(run_id=number, generation_id=number-1, seed=42,
                  rank_sha256='test', binary_sha256='test', checker='OK',
                  move_count=1, solver_seconds=999 if paused else .25, solver_exit=0,
                  settings={}, input_ranks=list(range(100)), moves=['sa'], solution_debug='',
                  completed_status=[], algorithms=[dict(id=0, name='candidate', moves=1,
                       seconds=999 if paused else .125, cpu_seconds=999 if paused else .1)])
    if paused:result['timing_valid']=False
    return result


class TimingPauseTests(unittest.TestCase):
    def test_stop_wakes_a_launch_waiting_for_resume_without_starting_it(self):
        runner.STOP_REQUESTED.clear()
        runner.PAUSED=True
        errors=[]
        def launch():
            try:runner.start_process(['unused'])
            except InterruptedError:errors.append('stopped')
        with patch.object(runner.subprocess,'Popen') as popen:
            worker=threading.Thread(target=launch)
            worker.start()
            try:
                time.sleep(.02)
                self.assertTrue(worker.is_alive())
                runner.stop_workers()
                worker.join(timeout=1)
                self.assertFalse(worker.is_alive())
                self.assertEqual(errors,['stopped'])
                popen.assert_not_called()
            finally:
                runner.PAUSED=False
                runner.STOP_REQUESTED.clear()

    def test_paused_record_round_trip_and_legacy_records(self):
        for paused in (False, True):
            value=record(paused=paused)
            self.assertEqual(runner.parse_records(runner.render_record(value)), [value])

    def test_all_summary_timing_samples_exclude_paused_runs(self):
        records=[record(),record(2,True)]
        tables=[]
        def capture(headers, rows):
            tables.append((headers,list(rows)));return ''
        with tempfile.TemporaryDirectory() as tmp, patch.object(runner,'table',side_effect=capture):
            config=dict(prefix='test',seed=42,size=100,generator=runner.GENERATOR,python_version='test')
            runner.save_summary(Path(tmp)/'summary.md',config,records,'complete')
        for headers, rows in tables:
            if 'Average solver seconds' in headers:
                self.assertEqual(rows[0][headers.index('Average solver seconds')], '0.250000')
            if 'Metric / algorithm' in headers:
                value=next(row for row in rows if row[0]=='Solver runtime')
                self.assertEqual(value[1:3], (1,'0.250'))
            if 'Total seconds Mean' in headers:
                self.assertEqual(rows[0][headers.index('Timed runs')],1)
                self.assertEqual(rows[0][headers.index('Total seconds Mean')],.25)
            if 'Seconds Mean' in headers:
                self.assertEqual(rows[0][headers.index('Runs')],2)
                self.assertEqual(rows[0][headers.index('Timed runs')],1)
                self.assertEqual(rows[0][headers.index('Seconds Mean')],.125)
                self.assertEqual(rows[0][headers.index('Mean CPU seconds')],.1)
            if 'Wall seconds' in headers:
                self.assertEqual(rows[1][headers.index('Wall seconds')],'—')
                self.assertEqual(rows[1][headers.index('CPU seconds')],'—')

    def test_run_case_marks_interrupted_measurement(self):
        process=Mock()
        process.wait.return_value=0
        process.returncode=0
        process.communicate.return_value=(b'OK',b'')
        def interrupt_read(*args):
            runner.PAUSE_EPOCH += 1
            return b''
        with patch.object(runner,'start_process',return_value=process), patch.object(runner.os,'read',side_effect=interrupt_read):
            result=runner.run_case([0,1],live_progress=False)
        self.assertEqual(result['checker'],'OK')
        self.assertFalse(result['timing_valid'])


if __name__=='__main__':unittest.main()
