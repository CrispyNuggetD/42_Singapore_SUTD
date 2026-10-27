import contextlib
import io
import json
import os
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch, Mock
import lan42_recommend as recommend

class Recommendations(unittest.TestCase):
    def test_missing_install_recommends_path_with_live_features(self):
        response = Mock()
        response.__enter__ = Mock(return_value=response)
        response.__exit__ = Mock(return_value=False)
        response.read.return_value = (recommend.START + '\n- `/who`: live feature.\n' + recommend.END).encode()
        output = io.StringIO()
        with tempfile.TemporaryDirectory() as tmp, patch.object(recommend.Path, 'home', return_value=Path(tmp)), \
             patch.dict(os.environ, {'LAN42_CLONE': '', 'LAN42_REPO_ROOT': ''}), \
             patch.object(recommend.urllib.request, 'urlopen', return_value=response), \
             patch.object(recommend.shutil, 'which', return_value=None), contextlib.redirect_stdout(output):
            recommend.recommend()
        text = output.getvalue()
        self.assertIn('live feature', text)
        self.assertIn('Documents/42sg-campus-lan', text)
        self.assertIn('git clone', text)
        self.assertIn('install tmux', text)

    def test_installed_custom_clone_skips_download_and_keeps_tmux_recommendation(self):
        with tempfile.TemporaryDirectory() as tmp, patch.dict(os.environ, {'LAN42_CLONE': '', 'LAN42_REPO_ROOT': ''}), \
             patch.object(recommend, 'feature_text') as features, patch.object(recommend.shutil, 'which', return_value='/bin/tmux'):
            (Path(tmp)/'lan42.sh').touch()
            output = io.StringIO()
            with contextlib.redirect_stdout(output):
                recommend.recommend({'LAN42_CLONE': tmp})
            features.assert_not_called()
            self.assertIn('LAN42 found', output.getvalue())
            self.assertIn('separate workspace', output.getvalue())
            self.assertNotIn('git clone', output.getvalue())

    def test_offline_snapshot_is_dated_and_validation_rejects_terminal_controls(self):
        with patch.object(recommend.urllib.request, 'urlopen', side_effect=OSError('offline')):
            features, source = recommend.feature_text()
        self.assertIn('offline snapshot', source)
        self.assertIn('/who', features)
        for text in ('missing', recommend.START+'\n- bad\x1b[2J\n'+recommend.END):
            with self.assertRaises(ValueError):
                recommend.excerpt(text)

    def test_refresh_is_exact_canonical_readme_excerpt(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root/'README.md').write_text(recommend.START+'\n- `/who`: test.\n'+recommend.END)
            with patch.object(recommend, 'CACHE', root/'cache.json'):
                recommend.refresh(root)
            self.assertEqual(json.loads((root/'cache.json').read_text())['features'], '- `/who`: test.')

if __name__ == '__main__':
    unittest.main()
