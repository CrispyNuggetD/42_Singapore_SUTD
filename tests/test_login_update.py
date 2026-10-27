"""Workflow handoff tests use an entirely isolated HOME and stub every task."""
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

SOURCE=(Path(__file__).resolve().parents[1]/'Useful .zshrc edits (addition)').resolve()

@unittest.skipUnless(shutil.which('zsh'),'zsh unavailable')
class WorkflowUpdate(unittest.TestCase):
    def exercise(self, mode, replace_entry=False, fail=False):
        with tempfile.TemporaryDirectory() as tmp:
            home=Path(tmp)
            (home/'settings').write_text('export ZSHRC_SETUP_DONE=1\nexport autoupdatezshrc='+mode+'\n')
            rc='''source "$SOURCE"
pullzshrc() { print PULL >> "$EVIDENCE"; return ${FAIL_PULL:-0}; }
synczshrc() { print SYNC >> "$EVIDENCE"; }
_privatebackup_check() { print PRIVATE >> "$EVIDENCE"; }
_dailylogin_settings() { :; }
_dailylogin_tasks() { print TASKS >> "$EVIDENCE"; }
refreshproject() { :; }
_daily_terminal() { print "TERMINAL:$*" >> "$EVIDENCE"; }
loginitems=FALSE
'''
            if replace_entry:
                rc+='dli() { print "LATEST_ENTRY:${*}:guard=${_DAILYLOGIN_REFRESHED:-0}" >> "$EVIDENCE"; }\n'
            (home/'.zshrc').write_text(rc)
            script='''source "$SOURCE"
pullzshrc() { print PULL >> "$EVIDENCE"; return ${FAIL_PULL:-0}; }
synczshrc() { print SYNC >> "$EVIDENCE"; }
_privatebackup_check() { print OLD_ENTRY_TASK >> "$EVIDENCE"; }
dli "one argument"
result=$?
wait
exit $result
'''
            env=dict(os.environ,HOME=tmp,ZDOTDIR=tmp,SOURCE=str(SOURCE),EVIDENCE=str(home/'events'),
                     ZSHRC_SETTINGS_FILE=str(home/'settings'),ZSHRC_RUNTIME_ROOT=tmp,
                     ZSHRC_PORTABLE_SETTINGS_FILE='',XDG_STATE_HOME=tmp,FAIL_PULL='9' if fail else '0',
                     LOGIN_STARTUP_SKIP='1')
            if mode=='FALSE':
                script='source "$HOME/.zshrc"\ndli "one argument"\nresult=$?\nwait\nexit $result\n'
            result=subprocess.run(['zsh','-f','-c',script],env=env,capture_output=True,text=True,timeout=10)
            events=(home/'events').read_text().splitlines() if (home/'events').exists() else []
            return result,events

    def test_entire_new_entry_replaces_old_workflow(self):
        result,events=self.exercise('TRUE',replace_entry=True)
        self.assertEqual(result.returncode,0,result.stderr+result.stdout)
        self.assertEqual(events,['PULL','SYNC','LATEST_ENTRY:one argument:guard=1'])

    def test_identical_workflow_pulls_once_and_runs_tasks_once(self):
        result,events=self.exercise('TRUE')
        self.assertEqual(result.returncode,0,result.stderr+result.stdout)
        for marker in ('PULL','SYNC','PRIVATE','TASKS'):self.assertEqual(events.count(marker),1,events)
        self.assertNotIn('OLD_ENTRY_TASK',events)
        self.assertEqual(sum(e=='TERMINAL:show-all' for e in events),1)

    def test_failed_pull_starts_no_tasks(self):
        result,events=self.exercise('TRUE',fail=True)
        self.assertNotEqual(result.returncode,0)
        self.assertEqual(events,['PULL'])

    def test_auto_update_off_does_not_pull(self):
        result,events=self.exercise('FALSE')
        self.assertEqual(result.returncode,0,result.stderr)
        self.assertNotIn('PULL',events)
        self.assertEqual(events.count('TASKS'),1)

if __name__=='__main__':unittest.main()
