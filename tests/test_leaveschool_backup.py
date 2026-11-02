import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import leaveschool


@unittest.skipUnless(shutil.which('zstd'), 'zstd is required for chat backups')
class ChatBackupCompression(unittest.TestCase):
    def test_large_chat_is_compressed_in_private_repo(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / 'sessions'
            source.mkdir()
            original = source / 'rollout.jsonl'
            original.write_bytes((b'{"message":"repeated backup text"}\n') * 50)
            private = root / '42_Singapore_SUTD_hnah_private'
            private.mkdir()
            main = root / 'school'
            main.mkdir()

            def fake_git(_repo, *args, **_kwargs):
                value = str(private) if args[0] == 'rev-parse' else 'git@github.com:owner/42_Singapore_SUTD_hnah_private.git'
                return SimpleNamespace(stdout=value + '\n')

            with patch.object(leaveschool, 'git', side_effect=fake_git), \
                 patch.object(leaveschool, 'MAX_CHAT_BLOB', 100):
                leaveschool.backup(source, private, main)
                leaveschool.backup(source, private, main)

            stored = private / '42/codex_cache/sessions/rollout.jsonl.zst'
            self.assertTrue(stored.is_file())
            self.assertFalse(stored.with_suffix('').exists())
            restored = subprocess.check_output(['zstd', '-dc', '--', str(stored)])
            self.assertEqual(restored, original.read_bytes())

    def test_codex_compressed_chat_stays_compressed_when_large(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / 'sessions'
            source.mkdir()
            raw = source / 'rollout.jsonl'
            content = (b'{"message":"compressed source"}\n') * 50
            raw.write_bytes(content)
            archive = source / 'rollout.jsonl.zst'
            with archive.open('wb') as output:
                subprocess.run(['zstd', '-q', '-c', '--', str(raw)], stdout=output, check=True)
            raw.unlink()
            private = root / '42_Singapore_SUTD_hnah_private'
            private.mkdir()
            main = root / 'school'
            main.mkdir()

            def fake_git(_repo, *args, **_kwargs):
                value = str(private) if args[0] == 'rev-parse' else 'git@github.com:owner/42_Singapore_SUTD_hnah_private.git'
                return SimpleNamespace(stdout=value + '\n')

            with patch.object(leaveschool, 'git', side_effect=fake_git), \
                 patch.object(leaveschool, 'MAX_CHAT_BLOB', 100):
                leaveschool.backup(source, private, main)

            stored = private / '42/codex_cache/sessions/rollout.jsonl.zst'
            self.assertEqual(subprocess.check_output(['zstd', '-dc', '--', str(stored)]), content)
            self.assertFalse(stored.with_suffix('').exists())
