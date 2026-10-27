import importlib.util
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import time
import unittest
from unittest.mock import Mock,patch

SOURCE=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(SOURCE/"tools"))
import app_manager as apps

class Apps(unittest.TestCase):
    def setUp(self):
        self.temp=tempfile.TemporaryDirectory();self.addCleanup(self.temp.cleanup)
        self.root=Path(self.temp.name)
        self.env=patch.dict(os.environ,{'XDG_CONFIG_HOME':str(self.root/'config'),'XDG_DATA_HOME':str(self.root/'data')})
        self.env.start();self.addCleanup(self.env.stop)
        self.binary=self.root/'portable app';self.binary.write_text('#!/bin/sh\nexit 0\n');self.binary.chmod(0o755)

    def test_default_off_and_saved_list_private(self):
        with patch.object(apps.subprocess,'Popen') as start:
            apps.launch_apps();start.assert_not_called()
        apps.save([dict(name='Custom',argv=[str(self.binary)])])
        self.assertEqual(apps.load()[0]['argv'],[str(self.binary)])
        self.assertEqual(apps.config_path().stat().st_mode&0o777,0o600)
        apps.save([]);self.assertEqual(apps.load(),[])

    def test_launch_uses_argument_list_no_shell_and_skips_running(self):
        app=dict(name='Custom',argv=[str(self.binary),'literal;$(id)'])
        apps.save([app])
        with patch.object(apps,'already_running',return_value=False),patch.object(apps.subprocess,'Popen') as start:
            apps.launch_apps();self.assertEqual(start.call_args.args[0],app['argv'])
            self.assertNotIn('shell',start.call_args.kwargs)
        with patch.object(apps,'already_running',return_value=True),patch.object(apps.subprocess,'Popen') as start:
            apps.launch_apps();start.assert_not_called()

    @patch.object(apps.sys, 'platform', 'linux')
    def test_missing_search_only_opens_with_explicit_yes(self):
        with patch.object(apps.sys.stdin,'isatty',return_value=True),patch('builtins.input',return_value=''),patch.object(apps.webbrowser,'open') as browser:
            apps.missing('Telegram');browser.assert_not_called()
        with patch.object(apps.sys.stdin,'isatty',return_value=True),patch('builtins.input',return_value='yes'),patch.object(apps.webbrowser,'open') as browser:
            apps.missing('Telegram');self.assertIn('q=install+telegram+ubuntu',browser.call_args.args[0])

    def test_default_binary_name_desktop_and_no_overwrite(self):
        app=dict(name='Telegram',argv=[str(self.binary)])
        target=apps.install_desktop(app,'telegram')
        self.assertEqual(target.name,'telegram.desktop')
        self.assertIn('Icon=telegram\n',target.read_text())
        with self.assertRaises(FileExistsError):apps.install_desktop(app,'other')
        apps.install_desktop(app,'other',overwrite=True)
        self.assertEqual(len(list(target.parent.glob('telegram.desktop.bak-*'))),1)

    def test_controls_and_path_traversal_cannot_inject_desktop_keys(self):
        with self.assertRaises(ValueError):apps.install_desktop(dict(name='bad\nExec=attack',argv=[str(self.binary)]),'icon')
        with self.assertRaises(ValueError):apps.install_desktop(dict(name='good',argv=[str(self.binary)]),'icon\nExec=attack')
        target=apps.install_desktop(dict(name='../../safe',argv=[str(self.binary)]),'icon')
        self.assertEqual(target.parent,self.root/'data/applications');self.assertEqual(target.name,'safe.desktop')

    def test_dock_pin_preserves_order(self):
        with patch.object(apps.subprocess,'run',side_effect=[Mock(stdout="['first.desktop', 'second.desktop']"),Mock()]) as run:
            self.assertTrue(apps.pin('telegram.desktop'))
            self.assertEqual(run.call_args.args[0][-1],"['first.desktop', 'second.desktop', 'telegram.desktop']")

    def test_exec_quoting_parses_with_gio(self):
        try:from gi.repository import Gio
        except ImportError:self.skipTest('Gio unavailable')
        # Parse the launcher with the actual desktop library, without launching apps.
        special=self.root/'app with $dollar and `ticks` and "quotes" and \\backslash'
        special.write_text('#!/usr/bin/python3\nimport json,sys\nfrom pathlib import Path\nPath(sys.argv[1]).write_text(json.dumps(sys.argv[2:]))\n');special.chmod(0o755)
        output=self.root/'argv.json'
        arguments=['a b','$not-expanded','%u','back\\slash']
        target=apps.install_desktop(dict(name='Escaping',argv=[str(special),str(output),*arguments]),'icon')
        info=Gio.DesktopAppInfo.new_from_filename(str(target))
        self.assertIsNotNone(info)
        info.launch([],None)
        for _ in range(50):
            if output.exists():break
            time.sleep(.02)
        self.assertEqual(json.loads(output.read_text()),arguments)

if __name__=='__main__':unittest.main()
