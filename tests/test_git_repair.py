"""Isolated Git recovery: local bare remotes; Codex always mocked."""
import importlib.util
import json
import os
from pathlib import Path
import subprocess
import tempfile
import time
import unittest
from unittest.mock import patch

ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('git_repair',ROOT/'tools/git_repair.py')
r=importlib.util.module_from_spec(spec);spec.loader.exec_module(r)


class RepairTests(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory();self.addCleanup(self.tmp.cleanup)
        self.root=Path(self.tmp.name);self.remote=self.root/'remote.git'
        self.run_git(self.root,'init','--bare',str(self.remote))
        self.repo=self.root/'local';self.other=self.root/'other'
        self.run_git(self.root,'clone',str(self.remote),str(self.repo))
        self.configure(self.repo)
        (self.repo/'base').write_text('base\n');self.commit(self.repo,'base')
        self.run_git(self.repo,'push','-u','origin','HEAD')
        self.run_git(self.root,'clone',str(self.remote),str(self.other));self.configure(self.other)
        self.state=self.root/'state'/'run'

    def run_git(self,repo,*args):
        return subprocess.run(['git','-C',str(repo),*args],check=True,capture_output=True,text=True).stdout.strip()

    def configure(self,repo):
        self.run_git(repo,'config','user.name','Fixture');self.run_git(repo,'config','user.email','fixture@example.invalid')

    def commit(self,repo,name):
        self.run_git(repo,'add','--',name);self.run_git(repo,'commit','-m','fixture')

    def diverge(self,conflict=False):
        name='base' if conflict else 'remote-only'
        (self.other/name).write_text('remote\n');self.commit(self.other,name);self.run_git(self.other,'push')
        name='base' if conflict else 'local-only'
        (self.repo/name).write_text('local\n');self.commit(self.repo,name)

    def test_clean_divergence_merges_without_rewriting_or_pushing(self):
        self.diverge();old=self.run_git(self.repo,'rev-parse','HEAD')
        remote_before=self.run_git(self.remote,'rev-parse','HEAD')
        with patch.object(r,'advise',return_value='merge_upstream'):
            result=r.repair(self.repo,self.state)
        self.assertTrue(result['ok'],result)
        self.run_git(self.repo,'merge-base','--is-ancestor',old,'HEAD')
        self.assertTrue((self.repo/'remote-only').exists())
        self.assertEqual(self.run_git(self.remote,'rev-parse','HEAD'),remote_before)
        self.assertFalse(self.run_git(self.repo,'status','--porcelain'))

    def test_conflict_is_left_in_temporary_worktree_only(self):
        self.diverge(True);old=self.run_git(self.repo,'rev-parse','HEAD')
        with patch.object(r,'advise',return_value='merge_upstream'):
            result=r.repair(self.repo,self.state)
        self.assertFalse(result['ok']);self.assertIn('conflicts',result['reason'])
        self.assertEqual(self.run_git(self.repo,'rev-parse','HEAD'),old)
        self.assertEqual((self.repo/'base').read_text(),'local\n')
        self.assertFalse((self.repo/'.git/MERGE_HEAD').exists())

    def test_dirty_worktree_is_preserved(self):
        (self.repo/'base').write_text('unsaved work\n')
        with patch.object(r,'advise',return_value='merge_upstream'):
            result=r.repair(self.repo,self.state)
        self.assertFalse(result['ok']);self.assertEqual((self.repo/'base').read_text(),'unsaved work\n')

    def test_stale_empty_lock_is_retained_as_private_backup(self):
        lock=self.repo/'.git/index.lock';lock.touch();os.utime(lock,(time.time()-700,)*2)
        original_run=r.subprocess.run
        def run(args,**kwargs):
            if args[0]=='ps':return subprocess.CompletedProcess(args,0,stdout='zsh\n',stderr='')
            return original_run(args,**kwargs)
        with patch.object(r,'advise',return_value='stale_index_lock'),patch.object(r.subprocess,'run',side_effect=run):
            result=r.repair(self.repo,self.state)
        self.assertTrue(result['ok'],result);self.assertFalse(lock.exists())
        backup=next((self.state/'backups').iterdir());self.assertEqual(backup.stat().st_mode&0o777,0o600)

    def test_fresh_lock_is_not_removed(self):
        lock=self.repo/'.git/index.lock';lock.touch()
        with patch.object(r,'advise',return_value='stale_index_lock'):
            result=r.repair(self.repo,self.state)
        self.assertFalse(result['ok']);self.assertTrue(lock.exists())

    def test_codex_timeout_is_reported_not_success(self):
        with patch.object(r,'advise',side_effect=subprocess.TimeoutExpired('codex',90)):
            result=r.repair(self.repo,self.state)
        self.assertFalse(result['ok']);self.assertIn('deadline',result['reason'])
        self.assertFalse(json.loads((self.state/'result.json').read_text())['ok'])

    def test_active_git_keeps_even_old_lock(self):
        lock=self.repo/'.git/index.lock';lock.touch();os.utime(lock,(time.time()-700,)*2)
        before=r.snapshot(self.repo)
        with patch.object(r.subprocess,'run',return_value=subprocess.CompletedProcess([],0,stdout='git\n',stderr='')):
            with self.assertRaisesRegex(ValueError,'Git process is active'):
                r.retire_lock(self.repo,before,self.state/'backups')
        self.assertTrue(lock.exists())

    def test_concurrent_change_aborts_before_merge(self):
        self.diverge()
        def advise(*args):
            (self.repo/'new-local-work').write_text('keep me')
            return 'merge_upstream'
        with patch.object(r,'advise',side_effect=advise):
            result=r.repair(self.repo,self.state)
        self.assertFalse(result['ok']);self.assertIn('changed while',result['reason'])
        self.assertEqual((self.repo/'new-local-work').read_text(),'keep me')

    def test_codex_receives_only_facts_in_read_only_temporary_directory(self):
        from unittest.mock import Mock
        task=self.root/'agent-task';task.mkdir()
        facts=dict(dirty=False,ahead=1,behind=1,index_lock=False,error_class='diverged')
        def launch(args,**kwargs):
            Path(args[args.index('--output-last-message')+1]).write_text('{"action":"merge_upstream"}')
            process=Mock(returncode=0)
            return process
        with patch.object(r.shutil,'which',return_value='/fake/codex'),patch.object(r.subprocess,'Popen',side_effect=launch) as start:
            self.assertEqual(r.advise(facts,task),'merge_upstream')
        args=start.call_args.args[0]
        self.assertEqual(args[args.index('--sandbox')+1],'read-only')
        self.assertIn('--ignore-user-config',args)
        self.assertEqual(args[args.index('--cd')+1],str(task))
        self.assertNotIn(str(self.repo), ' '.join(args))
