import importlib.util
import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
import portfolio

class Portfolio(unittest.TestCase):
    def test_focus_is_safe_and_matches_readme(self):
        c=portfolio.load('config.json')
        self.assertIn(portfolio.current_block(c),(portfolio.ROOT/'README.md').read_text())
        self.assertIn(portfolio.project_url(c['current_project']),portfolio.current_block(c))
        for bad in ('../private','/tmp','missing-project'):
            with self.assertRaises(ValueError):portfolio.checked_path(bad)
    def test_shell_import_reads_only_assignments_without_execution(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp);(root/'portfolio').mkdir();(root/'Core Curriculum/Projects/3_Philosophers').mkdir(parents=True)
            (root/'portfolio/config.json').write_text(json.dumps({'current_project':'Core Curriculum/Projects/3_Philosophers','projects':[]}))
            (root/'README.md').write_text(portfolio.START+'\n'+portfolio.END)
            settings=root/'settings.zsh';settings.write_text('export current_proj=3_Philosophers\nexport PROJECTS_ROOT="Core Curriculum/Projects"\ntouch SHOULD_NOT_RUN\nexport SECRET=not-published\n')
            with patch.object(portfolio,'ROOT',root):portfolio.refresh(settings)
            self.assertFalse((root/'SHOULD_NOT_RUN').exists())
            self.assertNotIn('SECRET',(root/'portfolio/config.json').read_text())
    def test_retries_keep_latest_validated_result_not_failed_or_highest_ever(self):
        detail={'teams':[{'final_mark':125,'validated?':True,'closed_at':'2026-01-01'},
                         {'final_mark':100,'validated?':True,'closed_at':'2026-02-01'},
                         {'final_mark':0,'validated?':False,'closed_at':'2026-03-01'}]}
        self.assertEqual(portfolio.validated_attempt(detail)['final_mark'],100)
        self.assertIsNone(portfolio.validated_attempt({'teams':[{'final_mark':0,'validated?':False}]}))
    @unittest.skipUnless(importlib.util.find_spec('markdown'),'Install Markdown==3.7')
    def test_build_contains_only_site_and_published_owner_fields(self):
        with tempfile.TemporaryDirectory() as tmp:
            out=Path(tmp);portfolio.build(out)
            self.assertEqual({p.name for p in out.iterdir()},{'index.html','current-project.html','how-i-work.html','assets','.nojekyll'})
            home=(out/'index.html').read_text()
            for needle in ('Christopher Nah','LAN42','push_swap','Philosophers','Last checked:','data-until=','AI-assisted, human-directed'):self.assertIn(needle,home)
            for bad in ('{{','client_secret','access_token','teammates','/Users/crispy'):self.assertNotIn(bad,home)
            self.assertIn(portfolio.project_url(portfolio.load('config.json')['current_project']),(out/'current-project.html').read_text())
            self.assertIn('iSH',(out/'how-i-work.html').read_text())
            self.assertIn('100/100 · API mark',home)
            self.assertIn('textbooks, research papers',home)
            self.assertIn(portfolio.REPO+'/blob/main/Core%20Curriculum/Projects/2_push_swap/README.md#appendix',home)
            self.assertIn('data-event-horizon',home)
            self.assertIn('data-gender="male"',home)
            self.assertIn('assets/event-horizon.js',home)
            self.assertIn('id="toolkit"',home)
            self.assertIn('Build your own astronaut scene',home)
            self.assertIn('Optional Event Horizon add-on',home)
            self.assertIn('class="eh-horizon-info"',home)
            self.assertNotIn('100/125',home)
