"""Full workspace restart tests mock all shutdown, GUI and launcher operations."""
import os
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import Mock,patch
import relogin as login
import workspace_restart as workspace

class Relogin(unittest.TestCase):
    def environment(self,tmp):
        return dict(HOME=tmp,XDG_RUNTIME_DIR=tmp,XDG_SESSION_ID='test',XDG_STATE_HOME=tmp,
                    TMUX='old-client',GNOME_TERMINAL_SCREEN='old-window')

    def test_restart_idle_no_question_and_busy_no_cancels(self):
        for jobs in ([],[dict(command='LAN42',tty='pts/1')]):
            with tempfile.TemporaryDirectory() as tmp,patch.dict(os.environ,self.environment(tmp)), \
                 patch.object(login.Path,'home',return_value=Path(tmp)), \
                 patch.object(login,'helper_paths',return_value=(Path(tmp)/'close.py',None)), \
                 patch.object(login,'active_terminal_jobs',return_value=jobs), \
                 patch.object(login,'force_consent',return_value=False) as consent, \
                 patch.object(login.subprocess,'Popen') as spawn:
                login.restart()
                self.assertEqual(consent.call_count,bool(jobs))
                self.assertEqual(spawn.call_count,not jobs)

    def test_busy_yes_schedules_full_restart(self):
        with tempfile.TemporaryDirectory() as tmp,patch.dict(os.environ,self.environment(tmp)), \
             patch.object(login.Path,'home',return_value=Path(tmp)), \
             patch.object(login,'helper_paths',return_value=(Path(tmp)/'close.py',None)), \
             patch.object(login,'active_terminal_jobs',return_value=[dict(command='python3',tty='pts/3')]), \
             patch.object(login,'force_consent',return_value=True),patch.object(login.subprocess,'Popen') as spawn:
            login.restart()
            self.assertIn('--force-terminals',spawn.call_args.args[0])
            self.assertTrue(spawn.call_args.kwargs['start_new_session'])

    def test_worker_stops_old_workspace_before_closing_and_launching_once(self):
        with tempfile.TemporaryDirectory() as tmp,patch.dict(os.environ,self.environment(tmp)), \
             patch.object(login.Path,'home',return_value=Path(tmp)), \
             patch.object(login,'helper_paths',return_value=(Path(tmp)/'close.py',None)), \
             patch.object(login,'terminal_targets',return_value=[]):
            calls=[]
            def stop(force=False):calls.append(('stop',force))
            def run(command,**kwargs):calls.append(('close',command));return Mock(returncode=0)
            def launch(state,launcher):calls.append(('launch',launcher));(state/'started').touch()
            with patch.object(workspace,'stop',side_effect=stop),patch.object(login.subprocess,'run',side_effect=run), \
                 patch.object(workspace,'launch_login',side_effect=launch):
                self.assertEqual(login.worker(force=True),0)
            self.assertEqual([c[0] for c in calls],['stop','close','launch'])

    def test_blocked_close_without_force_never_launches(self):
        with tempfile.TemporaryDirectory() as tmp,patch.dict(os.environ,self.environment(tmp)), \
             patch.object(login.Path,'home',return_value=Path(tmp)), \
             patch.object(login,'helper_paths',return_value=(Path(tmp)/'close.py',None)), \
             patch.object(workspace,'stop'),patch.object(login,'active_terminal_jobs',return_value=[]), \
             patch.object(login.subprocess,'run',return_value=Mock(returncode=1)), \
             patch.object(workspace,'launch_login') as launch:
            self.assertEqual(login.worker(force=False),1)
            launch.assert_not_called()

    def test_new_work_after_idle_preview_cancels_before_shutdown(self):
        with tempfile.TemporaryDirectory() as tmp,patch.dict(os.environ,self.environment(tmp)), \
             patch.object(login.Path,'home',return_value=Path(tmp)), \
             patch.object(login,'helper_paths',return_value=(Path(tmp)/'close.py',None)), \
             patch.object(login,'active_terminal_jobs',return_value=[{'command':'new-job'}]), \
             patch.object(workspace,'stop') as stop:
            self.assertEqual(login.worker(force=False),1)
            stop.assert_not_called()

    def test_tmux_jobs_now_count_as_running_work(self):
        rows=[dict(pid=10,ppid=1,sid=10,tty='pts/2',state='S',command='zsh'),
              dict(pid=11,ppid=10,sid=10,tty='pts/2',state='S',command='python3')]
        self.assertEqual([r['pid'] for r in login.find_active_jobs(rows,set(),{'pts/2'},999)], [11])

    def test_mac_state_needs_no_linux_runtime(self):
        with patch.object(login.sys,'platform','darwin'),patch.dict(os.environ,{'TMPDIR':'/tmp/example-mac'}):
            self.assertEqual(login.session_state().parent,Path('/tmp/example-mac'))

class Workspace(unittest.TestCase):
    def test_dead_pane_pids_and_read_only_log_are_not_active_work(self):
        output='10\t/dev/pts/1\tdailylogin\t0\n11\t/dev/pts/2\told-job\t1\n12\t/dev/pts/3\tlan42\t0\n'
        with patch.object(workspace.subprocess,'run',return_value=Mock(returncode=0,stdout=output)):
            self.assertEqual(workspace.pane_processes(work_only=True),([12],{'pts/3'}))
            self.assertEqual(workspace.pane_processes(),([10,12],{'pts/1','pts/3'}))

    def test_stop_kills_old_server_and_managed_node_not_worker(self):
        rows=[dict(pid=10,ppid=1),dict(pid=11,ppid=10),dict(pid=20,ppid=11)]
        with patch.object(workspace,'pane_processes',return_value=([10],{'pts/1'})), \
             patch.object(workspace,'processes',return_value=rows),patch.object(workspace.os,'getpid',return_value=20), \
             patch.object(workspace,'identity',return_value=('start','exe')), \
             patch.object(workspace,'managed_nodes',return_value=[(30,('start','exe'))]), \
             patch.object(workspace.subprocess,'run') as run,patch.object(workspace.os,'kill') as kill, \
             patch.object(workspace,'wait',return_value=[]):
            workspace.stop(force=True)
            self.assertIn('kill-server',run.call_args.args[0])
            self.assertEqual(kill.call_args.args[0],30)

    def test_mac_launch_clears_old_tmux_environment_and_uses_new_window(self):
        with tempfile.TemporaryDirectory() as tmp,patch.object(workspace.sys,'platform','darwin'), \
             patch.dict(os.environ,{'TMUX':'old','TMUX_PANE':'old','GNOME_TERMINAL_SCREEN':'old'}), \
             patch.object(workspace.subprocess,'run') as run:
            workspace.launch_login(Path(tmp))
            args=run.call_args.args[0]
            self.assertEqual(args[0],'osascript')
            self.assertIn('dli;',args[-1])
            self.assertNotIn('TMUX',run.call_args.kwargs['env'])
            self.assertTrue((Path(tmp)/'started').exists())

if __name__=='__main__':unittest.main()
