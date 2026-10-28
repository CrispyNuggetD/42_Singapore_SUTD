import os
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from sync_zshrc import backup_file


class ShellBackups(unittest.TestCase):
    def test_preserves_source_and_stores_private_unique_backups_away_from_home(self):
        with tempfile.TemporaryDirectory() as root:
            root=Path(root);source=root/'.zshrc';source.write_bytes(b'original configuration')
            with patch.dict(os.environ,{'XDG_STATE_HOME':str(root/'state')}):
                first=backup_file(source);second=backup_file(source)
            self.assertNotEqual(first,second)
            self.assertEqual(first.parent,root/'state/42-shell/backups/zshrc')
            self.assertEqual(first.read_bytes(),source.read_bytes())
            self.assertEqual(first.stat().st_mode & 0o777,0o600)
            self.assertEqual(first.parent.stat().st_mode & 0o777,0o700)
            self.assertFalse(list(root.glob('.zshrc.bak-*')))
