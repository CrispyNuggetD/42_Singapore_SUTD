import io
import unittest
from unittest.mock import Mock,patch
from session_jobs import SessionJobs,due

class Jobs(unittest.TestCase):
    def test_idle_or_locked_deadlines_and_once(self):
        self.assertFalse(due(2399,2099,False))
        self.assertTrue(due(2400,0,False))
        self.assertTrue(due(0,2100,False))
        self.assertFalse(due(9999,9999,True))

    @patch('session_jobs.solver_running',return_value=True)
    @patch('session_jobs.subprocess.check_output',return_value='0 python3 tests/run_random_tests.py')
    def test_solver_interval_and_no_overlap(self,*_):
        now=[0];jobs=SessionJobs(io.StringIO(),Mock(),clock=lambda:now[0])
        jobs.config=lambda:{'solver_gitpushswap':True}
        jobs.launch=Mock(return_value=Mock(poll=Mock(return_value=None)))
        jobs.tick(False);now[0]=2519;jobs.tick(False);jobs.launch.assert_not_called()
        now[0]=2520;jobs.tick(False);jobs.launch.assert_called_once_with('gitpushswap')
        now[0]=9999;jobs.tick(False);self.assertEqual(jobs.launch.call_count,1)

    @patch('session_jobs.subprocess.run')
    def test_idle_waits_for_backup_and_runs_once(self,run):
        run.return_value.stdout='2500000'
        jobs=SessionJobs(io.StringIO(),Mock(),clock=lambda:0)
        jobs.config=lambda:{'idle_leavesch':True};jobs.launch=Mock(return_value=Mock(poll=Mock(return_value=None)))
        jobs.tick(True,True);jobs.launch.assert_not_called()
        jobs.tick(True);jobs.launch.assert_called_once_with('leavesch')
        jobs.tick(True);self.assertEqual(jobs.launch.call_count,1)


    @patch('session_jobs.subprocess.run')
    def test_failed_leave_does_not_retry_until_actual_activity(self,run):
        now=[0];run.return_value.stdout='2500000'
        jobs=SessionJobs(io.StringIO(),Mock(),clock=lambda:now[0])
        jobs.config=lambda:{'idle_leavesch':True}
        process=Mock();process.poll.return_value=None
        jobs.launch=Mock(return_value=process)
        jobs.tick(True)
        process.returncode=1;process.poll.return_value=1
        for t in (60,180,3600):
            now[0]=t;jobs.tick(True)
        jobs.launch.assert_called_once_with('leavesch')
        # Even an active session must not rearm while its input is still idle.
        jobs.tick(False);self.assertEqual(jobs.launch.call_count,1)
        run.side_effect=OSError('idle source unavailable')
        jobs.tick(False);self.assertEqual(jobs.launch.call_count,1)
        run.side_effect=None;run.return_value.stdout='0';jobs.tick(False)
        run.return_value.stdout='2500000';process.poll.return_value=None
        jobs.tick(False);self.assertEqual(jobs.launch.call_count,2)

    @patch('session_jobs.subprocess.run')
    def test_successful_leave_does_not_repeat_while_session_is_active_but_idle(self,run):
        run.return_value.stdout='2500000'
        jobs=SessionJobs(io.StringIO(),Mock(),clock=lambda:0)
        jobs.config=lambda:{'idle_leavesch':True}
        process=Mock();process.poll.return_value=None
        jobs.launch=Mock(return_value=process)
        jobs.tick(False)
        process.returncode=0;process.poll.return_value=0
        jobs.tick(False);jobs.tick(False)
        jobs.launch.assert_called_once_with('leavesch')
