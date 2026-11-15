"""Exercise the shared launcher with fake Git; never download or run exam software."""
import os
from pathlib import Path
import pty
import subprocess
import tempfile
import unittest

SOURCE=Path(__file__).resolve().parents[1]/'Useful .zshrc edits (addition)'

class ExamPractice(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory(prefix='42-exampractice-')
        self.addCleanup(self.tmp.cleanup)
        self.home=Path(self.tmp.name)
        self.bin=self.home/'bin';self.bin.mkdir()
        git=self.bin/'git'
        git.write_text('''#!/bin/sh
printf '%s\\n' "$@" > "$HOME/git-called"
for destination do :; done
mkdir -p "$destination/.resources/main"
printf '# fixture\\n' > "$destination/.resources/main/menu.sh"
printf 'printf "practice:%%s:%%s\\n" "$PWD" "$1"\\n' > "$destination/exam.sh"
''')
        git.chmod(0o755)
        self.env=dict(os.environ,HOME=str(self.home),PATH=str(self.bin)+':'+os.environ['PATH'],XDG_STATE_HOME=str(self.home/'state'))
        self.env.pop('EXAM_PRACTICE_ROOT',None)
        s=SOURCE.read_text();self.function=s[s.index('exampractice() ('):s.index('\n# compile helper')]

    def run_launcher(self,command,answers=None):
        if answers is None:
            return subprocess.run(['zsh','-fc',self.function+'\n'+command],env=self.env,input='',text=True,capture_output=True)
        master,slave=pty.openpty()
        try:
            p=subprocess.Popen(['zsh','-fc',self.function+'\n'+command],env=self.env,stdin=slave,stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True)
            os.write(master,answers.encode())
            out,err=p.communicate(timeout=5)
            return subprocess.CompletedProcess(p.args,p.returncode,out,err)
        finally:
            os.close(master);os.close(slave)

    def test_clone_default_and_remember(self):
        r=self.run_launcher('exampractice', 'y\n\n');self.assertEqual(r.returncode,0,r.stderr)
        path=self.home/'Documents/42_examshell'
        self.assertIn('practice:'+str(path),r.stdout)
        saved=self.home/'state/42-shell/exampractice-path'
        self.assertEqual(saved.read_text().strip(),str(path))
        self.assertEqual(saved.stat().st_mode&0o777,0o600)
        (self.home/'git-called').unlink()
        r=self.run_launcher('exampractice again');self.assertEqual(r.returncode,0,r.stderr)
        self.assertIn(':again',r.stdout);self.assertFalse((self.home/'git-called').exists())

    def test_custom_path_with_spaces(self):
        path=self.home/'custom practice'
        r=self.run_launcher('exampractice --path "'+str(path)+'" hello','yes\n\n')
        self.assertEqual(r.returncode,0,r.stderr);self.assertIn('practice:'+str(path)+':hello',r.stdout)

    def test_decline_and_noninteractive_never_clone(self):
        for answers in ('n\n',None):
            r=self.run_launcher('exampractice',answers)
            self.assertNotEqual(r.returncode,0);self.assertFalse((self.home/'git-called').exists())

    def test_existing_destination_not_overwritten(self):
        path=self.home/'Documents/42_examshell';path.mkdir(parents=True)
        keep=path/'keep.txt';keep.write_text('keep')
        r=self.run_launcher('exampractice','y\n\n')
        self.assertNotEqual(r.returncode,0);self.assertEqual(keep.read_text(),'keep')
        self.assertFalse((self.home/'git-called').exists())

if __name__=='__main__':unittest.main()
