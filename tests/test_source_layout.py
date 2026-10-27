"""Relocated sources must still update existing flat runtimes and sparse clones."""
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest
from unittest.mock import patch
import sync_zshrc

ROOT=Path(__file__).resolve().parents[1]

class SourceLayout(unittest.TestCase):
    def test_organized_checkout_installs_real_helpers_into_flat_runtime(self):
        with tempfile.TemporaryDirectory() as tmp:
            runtime=Path(tmp)/'runtime'
            with patch.object(sync_zshrc,'refresh_gobreak'):
                sync_zshrc.install_runtime(ROOT,runtime)
            for name in sync_zshrc.RUNTIME_FILES:
                source=ROOT/name if name=='Useful .zshrc edits (addition)' else ROOT/'tools'/name
                self.assertEqual((runtime/name).read_bytes(),source.read_bytes(),name)
            self.assertNotIn('Stable entry point',(runtime/'sync_zshrc.py').read_text())
            with patch.object(sync_zshrc,'refresh_gobreak'):
                sync_zshrc.install_runtime(runtime,Path(tmp)/'second-runtime')
            self.assertEqual((Path(tmp)/'second-runtime/setup_zshrc.py').read_bytes(),(runtime/'setup_zshrc.py').read_bytes())

    def test_sync_keeps_personal_content_and_installs_from_organized_checkout(self):
        with tempfile.TemporaryDirectory() as tmp:
            target=Path(tmp)/'.zshrc';target.write_text('# personal marker\nexport MY_SETTING=keep\n')
            with patch.object(sync_zshrc,'refresh_gobreak'):
                sync_zshrc.sync(ROOT/'Useful .zshrc edits (addition)',target,runtime=Path(tmp)/'runtime')
            self.assertTrue(target.read_text().startswith('# personal marker\nexport MY_SETTING=keep\n'))
            self.assertTrue((Path(tmp)/'runtime/daily_terminal.py').exists())

    def test_root_entry_bootstraps_existing_standalone_sparse_clone(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp);(root/'tools').mkdir()
            shutil.copyfile(ROOT/'install_gobreak.py',root/'install_gobreak.py')
            for source in (ROOT/'tools').glob('*.py'):shutil.copyfile(source,root/'tools'/source.name)
            def git(*args):subprocess.run(['git','-C',str(root),*args],check=True,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
            git('init');git('add','.');git('-c','user.name=Test','-c','user.email=test@example.invalid','commit','-m','fixture')
            git('sparse-checkout','set','--no-cone','/install_gobreak.py')
            self.assertFalse((root/'tools/install_gobreak.py').exists())
            result=subprocess.run(['python3',str(root/'install_gobreak.py'),'--help'],capture_output=True,text=True)
            self.assertEqual(result.returncode,0,result.stderr)
            self.assertTrue((root/'tools/install_gobreak.py').exists())
            self.assertIn('--repo',result.stdout)
