"""Shell summary/retry tests; no actual daily tasks or Codex are launched."""
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

SOURCE=(Path(__file__).resolve().parents[1]/'Useful .zshrc edits (addition)').read_text()
HELPERS=SOURCE[SOURCE.index('_dailylogin_run() {'):SOURCE.index('_dailylogin_chrome() {')]


@unittest.skipUnless(shutil.which('zsh'),'zsh unavailable')
class DailyRecovery(unittest.TestCase):
    def exercise(self, ok, retry_fails=False):
        with tempfile.TemporaryDirectory() as tmp:
            (Path(tmp)/'result.json').write_text(json.dumps(dict(ok=ok,reason='fixture diagnosis')))
            script=HELPERS+'''
_dailylogin_enabled() { [[ "$1" == TRUE ]]; }
_dailylogin_log() { print -- "$*"; }
_dailylogin_git_start() { dailylogin_git_pid=$$; dailylogin_git_state="$CASE_ROOT"; print QUEUED; }
main() {
 local -i dailylogin_done=0 dailylogin_skipped=0 dailylogin_failed=0
 local -i dailylogin_git_failed_index=0 dailylogin_git_retried=0
 local dailylogin_git_pid='' dailylogin_git_state=''
 local -a dailylogin_failures
 local syncmainrepo=TRUE
 syncproj() { if (( dailylogin_git_retried )) && [[ "$RETRY_FAILS" != 1 ]]; then print RETRIED; return 0; fi; return 9; }
 _dailylogin_run 'Main sync' syncmainrepo syncproj || true
 print OTHER_TASKS_CONTINUE
 _dailylogin_git_finish
 _dailylogin_summary
 print -- "COUNTS:$dailylogin_done:$dailylogin_failed"
}
main
'''
            return subprocess.run(['zsh','-f','-c',script],env=dict(os.environ,CASE_ROOT=tmp,RETRY_FAILS='1' if retry_fails else '0'),capture_output=True,text=True,timeout=5)

    def test_successful_repair_retries_full_sync_and_clears_original_failure(self):
        result=self.exercise(True)
        self.assertEqual(result.returncode,0,result.stderr)
        self.assertLess(result.stdout.index('QUEUED'),result.stdout.index('OTHER_TASKS_CONTINUE'))
        self.assertLess(result.stdout.index('OTHER_TASKS_CONTINUE'),result.stdout.index('RETRIED'))
        self.assertIn('COUNTS:1:0',result.stdout)
        self.assertIn('All enabled dailylogin tasks completed successfully',result.stdout)

    def test_unresolved_repair_stays_failed(self):
        result=self.exercise(False)
        self.assertEqual(result.returncode,0,result.stderr)
        self.assertIn('COUNTS:0:1',result.stdout)
        self.assertIn('fixture diagnosis',result.stdout)
        self.assertIn('Main sync (syncproj): exit 9',result.stdout)
        self.assertNotIn('All enabled dailylogin tasks completed successfully',result.stdout)

    def test_failed_full_retry_is_not_success(self):
        result=self.exercise(True,True)
        self.assertEqual(result.returncode,0,result.stderr)
        self.assertIn('COUNTS:0:1',result.stdout)
        self.assertEqual(result.stdout.count('QUEUED'),1)
        self.assertNotIn('All enabled dailylogin tasks completed successfully',result.stdout)
