#!/usr/bin/env python3
"""Build the public portfolio without changing personal shell settings or dailylogin."""
import argparse
from datetime import datetime, timezone
from html import escape
import json
import hashlib
import os
from pathlib import Path
import re
import shlex
import shutil
import sys
import subprocess
import urllib.request
from urllib.parse import quote

ROOT=Path(__file__).resolve().parent
REPO='https://github.com/CrispyNuggetD/42_Singapore_SUTD'
PAGES='https://crispynuggetd.github.io/42_Singapore_SUTD/'
START='<!-- portfolio-current:start -->';END='<!-- portfolio-current:end -->'

def load(name):return json.loads((ROOT/'portfolio'/name).read_text())
def checked_path(value):
    path=Path(value)
    if path.is_absolute() or '..' in path.parts or not path.parts or not (ROOT/path).is_dir():raise ValueError('Current project must be an existing repository-relative directory.')
    return path.as_posix()
def project_url(path):
    path=checked_path(path)
    return REPO+('/blob/main/' if (ROOT/path/'README.md').exists() else '/tree/main/')+quote(path+('/README.md' if (ROOT/path/'README.md').exists() else ''),safe='/')
def current_block(config):
    path=checked_path(config['current_project']);name=next((p['name'] for p in config['projects'] if p['path']==path),Path(path).name)
    return START+'\n**Current project: ['+name+']('+project_url(path)+')** · [What I’m working on now]('+PAGES+'#current-project)\n'+END

def refresh(settings=None):
    config=load('config.json')
    if settings:
        values={}
        for line in Path(settings).expanduser().read_text().splitlines():
            parts=shlex.split(line,comments=True)
            if len(parts)==2 and parts[0]=='export' and '=' in parts[1]:
                key,value=parts[1].split('=',1)
                if key in ('PROJECTS_ROOT','current_proj'):values[key]=value
        if not values.get('current_proj'):raise ValueError('No explicit current_proj found; nothing changed.')
        config['current_project']=checked_path(str(Path(values.get('PROJECTS_ROOT','Core Curriculum/Projects'))/values['current_proj']))
        (ROOT/'portfolio/config.json').write_text(json.dumps(config,indent=2)+'\n')
    text=(ROOT/'README.md').read_text()
    if START not in text or END not in text:raise ValueError('README current-project markers missing.')
    (ROOT/'README.md').write_text(re.sub(re.escape(START)+'.*?'+re.escape(END),lambda _:current_block(config),text,flags=re.S))

def validated_attempt(detail):
    candidates=[team for team in detail.get('teams',[]) if team.get('validated?') is True and type(team.get('final_mark')) is int]
    return max(candidates,key=lambda team:team.get('closed_at') or team.get('updated_at') or '') if candidates else None


def refresh_api(lan42):
    # Fixed owner, existing approved local operator and SG-scoped client only.
    sys.path.insert(0,str(Path(lan42).expanduser().resolve()))
    from campus_lan.access import Policy
    from campus_lan.campus_info import API
    if Policy().local_operator()!=234449:raise ValueError('Only the enrolled hnah API operator can publish this owner snapshot.')
    api=API();person=api.member_profile('hnah')
    if person.get('id')!=234449:raise ValueError('Owner identity mismatch.')
    cursus=next((c for c in person.get('cursus_users',[]) if c.get('cursus_id')==21),{})
    rows=[]
    for item in person.get('projects_users',[]):
        if 21 not in item.get('cursus_ids',[]):continue
        project=item.get('project',{})
        row=dict(slug=project.get('slug'),name=project.get('name'),mark=item.get('final_mark'),validated=item.get('validated?'),status=item.get('status'))
        if item.get('status')=='finished':
            identifier=item.get('id')
            if type(identifier) is not int or identifier<=0:raise ValueError('Invalid own project identifier.')
            # IDs come exclusively from the verified owner's profile. This local
            # exporter adds a fixed own-project detail read, not a general proxy.
            from campus_lan.api_pacing import wait
            wait()
            request=urllib.request.Request('https://api.intra.42.fr/v2/projects_users/'+str(identifier),headers={'Accept':'application/json','Authorization':'Bearer '+api.token,'User-Agent':'hnah-portfolio-own-project-check/1.0'})
            with api.opener.open(request,timeout=30) as response:detail=json.loads(response.read(2*1024*1024))
            if detail.get('user',{}).get('id')!=234449:raise ValueError('Project detail owner mismatch.')
            teams=detail.get('teams',[])
            latest=validated_attempt(detail)
            if latest:
                row.update(mark=latest['final_mark'],validated=True,mark_basis='Latest validated team attempt; failed retries do not erase a validated result')
            else:row['mark_basis']='API project summary; no validated team attempt listed'
            row['attempts_checked']=len(teams)
            row['marked_at']=detail.get('marked_at')
        rows.append(row)
    data=dict(login='hnah',observed_at=datetime.now(timezone.utc).isoformat(),source='42 API, owner-authorized single-user read',projects=rows,level=cursus.get('level'),begin_at=cursus.get('begin_at'),blackholed_at=cursus.get('blackholed_at'))
    (ROOT/'portfolio/progress.json').write_text(json.dumps(data,indent=2)+'\n')


def protect_external_links(document):
    """Keep Campus HQ navigation local; external HTTP(S) links get a new tab."""
    from html.parser import HTMLParser
    from urllib.parse import urlsplit
    offsets=[0]
    for line in document.splitlines(keepends=True):offsets.append(offsets[-1]+len(line))
    edits=[]
    class Links(HTMLParser):
        def handle_starttag(self,tag,attrs):
            if tag!='a':return
            attributes=dict(attrs);url=urlsplit(attributes.get('href') or '')
            if url.scheme and url.scheme not in ('http','https'):return
            external=bool(url.netloc and url.netloc.lower()!='crispynuggetd.github.io')
            raw=self.get_starttag_text()
            cleaned=re.sub(r"\s+target\s*=\s*(?:\"[^\"]*\"|'[^']*'|[^\s>]+)",'',raw,flags=re.I)
            if external:
                cleaned=re.sub(r"\s+rel\s*=\s*(?:\"[^\"]*\"|'[^']*'|[^\s>]+)",'',cleaned,flags=re.I)
                rel=set((attributes.get('rel') or '').split())|{'noopener','noreferrer'}
                cleaned=cleaned[:-1]+' target="_blank" rel="'+escape(' '.join(sorted(rel)),quote=True)+'"'+(' title="Opens in a new tab"' if 'title' not in attributes else '')+'>'
            line,column=self.getpos();begin=offsets[line-1]+column;edits.append((begin,begin+len(raw),cleaned))
    Links(convert_charrefs=True).feed(document)
    for begin,end,replacement in reversed(edits):document=document[:begin]+replacement+document[end:]
    return document

def render_docs(source, text=None):
    import markdown
    html=markdown.markdown((ROOT/source).read_text() if text is None else text,extensions=['tables','fenced_code','toc','md_in_html'])
    def links(match):
        url=match[1]
        if url.startswith(('https:','http:','#','mailto:')):return match[0]
        target=Path(os.path.normpath(str(Path(source).parent/url)))
        return 'href="'+REPO+'/blob/main/'+quote(str(target),safe='/#%')+'"'
    return re.sub(r'href="([^\"]+)"',links,html)

def build(output):
    config=load('config.json');progress=load('progress.json');checked_path(config['current_project'])
    if current_block(config) not in (ROOT/'README.md').read_text():raise ValueError('README focus is stale. Run python3 portfolio.py refresh first.')
    output=Path(output);output.mkdir(parents=True,exist_ok=True)
    shutil.copytree(ROOT/'portfolio/assets',output/'assets',dirs_exist_ok=True)
    marks={p['slug']:p for p in progress['projects']}
    def score(slug):
        row=marks.get(slug,{})
        if row.get('validated') is True and row.get('mark') is not None:
            project=next((p for p in config['projects'] if p['slug']==slug),{})
            maximum=project.get('max_mark')
            value=str(row['mark'])+'/'+str(maximum) if type(maximum) is int and row['mark']<=maximum else str(row['mark'])+'%'
            return value+' · API mark'
        return 'Not validated' if row.get('status')=='finished' else 'In progress / not graded'
    cards=[]
    for p in config['projects']:
        cards.append('<article class="project"><div class="label">'+escape(score(p['slug']))+'</div><h3><a href="'+project_url(p['path'])+'">'+escape(p['name'])+' ↗</a></h3><p>'+escape(p['summary'])+'</p></article>')
    current=next((p for p in config['projects'] if p['path']==config['current_project']),dict(name=Path(config['current_project']).name,summary='Current learning project.'))
    observed=progress.get('observed_at') or '';deadline=progress.get('blackholed_at') or ''
    def date(value):
        if not value:return 'Not available'
        from zoneinfo import ZoneInfo
        return datetime.fromisoformat(value.replace('Z','+00:00')).astimezone(ZoneInfo('Asia/Singapore')).strftime('%d %b %Y, %H:%M SGT')
    completed=sum(p.get('validated') is True for p in progress['projects'])
    template=(ROOT/'portfolio/index.html').read_text()
    values={'CURRENT_NAME':escape(current['name']),'CURRENT_URL':project_url(config['current_project']),'CURRENT_SUMMARY':escape(current['summary']),'PROJECTS':''.join(cards),'OBSERVED':escape(date(observed)),'OBSERVED_RAW':escape(observed,quote=True),'DEADLINE':escape(date(deadline)),'DEADLINE_RAW':escape(deadline,quote=True),'LEVEL':escape(str(progress.get('level') or 'Not available')),'COMPLETED':str(completed),'BEGIN':escape(date(progress.get('begin_at')))}
    disclosure=(ROOT/'README.md').read_text().split('**AI-assisted, human-directed.**',1)[1].split('\n## Say hi',1)[0]
    values['AI_DISCLOSURE']=render_docs('README.md','**AI-assisted, human-directed.**'+disclosure)
    # Repository curriculum groups, not inferred official milestone deadlines.
    groups=[('M.0',['42cursus-libft']),('M.1',['42cursus-ft_printf','42cursus-get_next_line','born2beroot']),('M.2',['42cursus-fdf','pipex','42cursus-push_swap','exam-rank-02']),('M.3',['42cursus-philosophers','42cursus-minishell','exam-rank-03'])]
    milestones=[]
    for label,slugs in groups:
        rows=[marks.get(slug,{}) for slug in slugs]
        complete=all(row.get('validated') is True for row in rows)
        dates=[row.get('marked_at') for row in rows if row.get('marked_at')]
        when=date(max(dates)).split(',')[0] if complete and dates else 'In progress' if label=='M.3' else 'Date unavailable'
        names=', '.join(row.get('name',slug) for row,slug in zip(rows,slugs))
        milestones.append('<li class="'+('complete' if complete else 'current')+'"><strong>'+label+(' ✓' if complete else ' · now')+'</strong><span>'+escape(when)+'</span><small>'+escape(names)+'</small></li>')
    values['MILESTONES']=''.join(milestones)
    values['TOOLKIT']=(ROOT/'portfolio/toolkit.html').read_text().replace('{{TOOLKIT_GUIDE}}',render_docs('docs/toolkit.md'))
    values['START_RAW']=escape(progress.get('begin_at') or '',quote=True)
    scene=(ROOT/'portfolio/horizon.html').read_text()
    for key,value in values.items():scene=scene.replace('{{'+key+'}}',value)
    template=template.replace('{{HORIZON}}',scene)
    for key,value in values.items():template=template.replace('{{'+key+'}}',value)
    (output/'index.html').write_text(template)
    destination=escape(project_url(config['current_project']),quote=True)
    (output/'current-project.html').write_text('<!doctype html><html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>Current project · hnah</title><p>Current project README (opens in a new tab): <a href="'+destination+'">'+escape(current['name'])+'</a></p><p><a href="index.html">Back to portfolio</a></p></html>')
    shell=(ROOT/'portfolio/document.html').read_text()
    for source,filename,title in [('docs/how-i-work.md','how-i-work.html','Candid 42 tips & workflow')]:
        (output/filename).write_text(shell.replace('{{TITLE}}',escape(title)).replace('{{CONTENT}}',render_docs(source)))
    # Website build identity is independent of runtime/server release numbers.
    built=datetime.now(timezone.utc)
    sha=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip()
    run=os.environ.get('GITHUB_RUN_NUMBER','')
    run_id=os.environ.get('GITHUB_RUN_ID','')
    label=('Build #'+run if run.isdigit() else 'Local preview')+' · '+sha[:7]
    metadata=dict(id=sha+':'+built.isoformat(),built_at=built.isoformat(),commit=sha,label=label)
    (output/'build.json').write_text(json.dumps(metadata,indent=2)+'\n')
    history=REPO+'/actions/runs/'+run_id if run_id.isdigit() else REPO+'/actions/workflows/portfolio-pages.yml'
    stamp='<div class="site-build" data-build-id="'+escape(metadata['id'],quote=True)+'" data-built-at="'+escape(metadata['built_at'],quote=True)+'"><span><a href="'+REPO+'/commit/'+sha+'">'+escape(label)+'</a> · Built '+escape(date(metadata['built_at']))+' · <a href="'+history+'">Deployment history ↗</a></span><span class="build-state" role="status">Website build details</span><button class="check-build" type="button" hidden>Check for updates</button></div>'
    # Version CSS/JS by content so cached assets cannot leave new markup unstyled.
    for page in output.glob('*.html'):
        def revision(match):
            asset=output/match[2]
            digest=hashlib.sha256(asset.read_bytes()).hexdigest()[:12]
            return match[1]+'="'+match[2]+'?v='+digest+'"'
        page.write_text(re.sub(r'(href|src)="(assets/[^"?]+\.(?:css|js))"',revision,protect_external_links(page.read_text().replace('{{BUILD_STAMP}}',stamp))))
    (output/'.nojekyll').touch()

def main():
    parser=argparse.ArgumentParser(description=__doc__);sub=parser.add_subparsers(dest='action',required=True)
    p=sub.add_parser('refresh');p.add_argument('--from-shell',action='store_true');p.add_argument('--settings',type=Path)
    p=sub.add_parser('build');p.add_argument('--output',type=Path,default=ROOT/'_site')
    p=sub.add_parser('refresh-api');p.add_argument('--lan42',type=Path,required=True)
    args=parser.parse_args()
    if args.action=='refresh':refresh(args.settings or Path(os.environ.get('ZSHRC_SETTINGS_FILE',str(Path.home()/'.42-shell-settings.zsh'))) if args.from_shell or args.settings else None)
    elif args.action=='refresh-api':refresh_api(args.lan42)
    else:build(args.output)
if __name__=='__main__':main()
