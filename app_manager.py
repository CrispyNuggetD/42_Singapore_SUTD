#!/usr/bin/env python3
"""Personal opt-in login apps and user-level Ubuntu desktop launchers."""
import argparse
import ast
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys
import tempfile
import urllib.parse
import webbrowser

PRESETS = {'telegram': ('Telegram', ['telegram-desktop', 'telegram', 'Telegram', 'telegram-launch']),
           'slack': ('Slack', ['slack'])}


def config_path():
    return Path(os.environ.get('XDG_CONFIG_HOME', str(Path.home()/'.config')))/'42-shell/apps.json'


def validate(app):
    if not isinstance(app, dict) or not isinstance(app.get('name'), str) or not app['name'].strip():
        raise ValueError('App needs a name.')
    argv=app.get('argv')
    if not isinstance(argv,list) or not argv or any(not isinstance(a,str) or not a or any(ord(c)<32 for c in a) for a in argv):
        raise ValueError('App command needs nonempty arguments without control characters.')
    if any(ord(c)<32 for c in app['name']):raise ValueError('Invalid app name.')
    return app


def load():
    try:data=json.loads(config_path().read_text())
    except FileNotFoundError:return []
    if not isinstance(data,list) or len(data)>50:raise ValueError('Invalid app list (maximum 50).')
    return [validate(app) for app in data]


def save(apps):
    if len(apps)>50:raise ValueError('Maximum 50 apps.')
    for app in apps:validate(app)
    path=config_path();path.parent.mkdir(parents=True,exist_ok=True,mode=0o700)
    temporary=None
    try:
        with tempfile.NamedTemporaryFile('w',dir=path.parent,delete=False) as output:
            temporary=Path(output.name);json.dump(apps,output,indent=2);output.write('\n')
        temporary.replace(path)
    finally:
        if temporary:temporary.unlink(missing_ok=True)


def resolve(argv):
    command=os.path.expanduser(argv[0])
    candidates=PRESETS.get(command.lower(),('',[command]))[1] if len(argv)==1 else [command]
    for candidate in candidates:
        executable=shutil.which(candidate)
        if not executable:
            local=Path.home()/'.local/bin'/candidate
            if local.is_file() and os.access(local,os.X_OK):executable=str(local)
        if executable:return [str(Path(executable).absolute()),*argv[1:]]
    return None


def missing(name, interactive=True):
    query='install '+name.lower()+' ubuntu'
    url='https://www.google.com/search?'+urllib.parse.urlencode({'q':query})
    print(name+' is not installed or its executable is not on PATH.')
    print('Install help: '+url)
    print('Campus tip: HOME has about 5 GB. Keep large portable apps in your own sgoinfre folder,')
    print('for example /sgoinfre/goinfre/Perso/'+__import__('getpass').getuser()+'/apps; then configure the executable path.')
    if interactive and sys.stdin.isatty() and input('Open this installation search in your browser? [y/N]: ').strip().lower() in ('y','yes'):
        webbrowser.open(url)


def already_running(argv):
    # Compare this user's process executable, not arbitrary matching command text.
    executable=Path(argv[0]).resolve()
    for entry in Path('/proc').iterdir():
        if not entry.name.isdecimal():continue
        try:
            if entry.stat().st_uid==os.getuid() and (entry/'exe').resolve(strict=True)==executable:
                command=(entry/'cmdline').read_bytes().split(b'\0')
                arguments=[os.fsdecode(a) for a in command[1:] if a]
                if len(argv)==1 or arguments[:len(argv)-1]==argv[1:]:return True
        except (OSError,RuntimeError):continue
    # Small personal launch scripts (e.g. telegram-launch) exec a differently named binary.
    name=Path(argv[0]).name
    if name in ('telegram-launch','telegram','telegram-desktop','Telegram'):name='Telegram'
    if name in ('Telegram','slack'):
        return subprocess.run(['pgrep','-u',str(os.getuid()),'-x',name],stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL).returncode==0
    return False


def launch_apps():
    apps=load()
    if not apps:
        print('Extra login apps: disabled (empty list). Run setupapps to opt in.');return
    failed=[]
    for app in apps:
        argv=resolve(app['argv'])
        if not argv:missing(app['name']);failed.append(app['name']);continue
        if already_running(argv):print(app['name']+' is already running; keeping its windows.');continue
        try:
            subprocess.Popen(argv,stdin=subprocess.DEVNULL,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL,start_new_session=True)
            print('Requested '+app['name']+'.')
        except OSError:print('Could not launch '+app['name']+'.');failed.append(app['name'])
    if failed:raise ValueError('Unavailable apps: '+', '.join(failed)+'. Use setupapps to change their paths.')


def app_from_command(command, name=None):
    # A real executable path containing spaces can be entered without shell quoting.
    path=Path(command).expanduser()
    argv=[str(path)] if path.is_file() else shlex.split(command)
    if not argv:raise ValueError('Enter an executable or command.')
    label=name or PRESETS.get(argv[0].lower(),(Path(argv[0]).stem,))[0]
    return validate(dict(name=label,argv=argv))


def configure():
    apps=load()
    print('Extra login apps are OFF by default. Your list stays local, outside Git.')
    print('Commands: list, add, remove, clear, done. Examples: telegram, slack, /sgoinfre/.../Telegram')
    print('Save large portable apps in your own sgoinfre folder; campus HOME has about 5 GB.')
    while True:
        print('\nLogin apps: '+(', '.join(a['name'] for a in apps) or '(none)'))
        action=input('Action [done]: ').strip().lower() or 'done'
        if action in ('done','q','quit'):save(apps);print('Saved. dailylogin will open this list; clear it to disable.');return
        if action=='list':continue
        if action=='clear':apps=[];continue
        if action=='remove':
            name=input('App name to remove: ').strip().lower();apps=[a for a in apps if a['name'].lower()!=name];continue
        if action!='add':print('Choose list, add, remove, clear, or done.');continue
        try:
            app=app_from_command(input('Binary/path and optional arguments [telegram]: ').strip() or 'telegram')
            if not resolve(app['argv']):missing(app['name'])
            app['name']=input('Display name ['+app['name']+']: ').strip() or app['name']
            validate(app)
            apps=[a for a in apps if a['name'].lower()!=app['name'].lower()]+[app]
        except ValueError as error:print(error)


def exec_quote(argument):
    # Desktop Entry escaping happens in two passes: string then Exec quoting.
    if any(ord(c)<32 for c in argument):raise ValueError('Control characters are not allowed.')
    value=argument.replace('%','%%')
    value=''.join('\\'+c if c in '\\"`$' else c for c in value)
    return '"'+value.replace('\\','\\\\')+'"'


def pin(desktop_id):
    try:
        result=subprocess.run(['gsettings','get','org.gnome.shell','favorite-apps'],capture_output=True,text=True,check=True,timeout=10).stdout.strip()
        favorites=ast.literal_eval(result.removeprefix('@as '))
        if not isinstance(favorites,list) or any(not isinstance(v,str) for v in favorites):return False
        if desktop_id not in favorites:
            subprocess.run(['gsettings','set','org.gnome.shell','favorite-apps',repr([*favorites,desktop_id])],capture_output=True,text=True,check=True,timeout=10)
        return True
    except (OSError,ValueError,SyntaxError,subprocess.SubprocessError):return False


def install_desktop(app, icon, terminal=False, overwrite=False):
    validate(app)
    argv=resolve(app['argv'])
    if not argv:raise ValueError('Executable unavailable; install it or enter its full path first.')
    if '=' in argv[0]:raise ValueError('Desktop executable paths cannot contain =.')
    if not icon or any(ord(c)<32 for c in icon):raise ValueError('Enter an icon theme name or image path.')
    if icon.startswith(('~','/')):
        icon=str(Path(icon).expanduser().resolve(strict=True))
        if not Path(icon).is_file():raise ValueError('Icon must be an image file.')
    slug=re.sub(r'[^a-z0-9_-]+','-',app['name'].lower()).strip('-')
    if not slug:raise ValueError('Name needs at least one letter or digit.')
    root=Path(os.environ.get('XDG_DATA_HOME',str(Path.home()/'.local/share')))/'applications'
    root.mkdir(parents=True,exist_ok=True)
    target=root/(slug+'.desktop')
    content='[Desktop Entry]\nType=Application\nName='+app['name'].replace('\\','\\\\')+'\n'
    content+='Exec='+' '.join(exec_quote(a) for a in argv)+'\nIcon='+icon.replace('\\','\\\\')+'\n'
    content+='Terminal='+str(terminal).lower()+'\nCategories=Utility;\nStartupNotify=true\n'
    # O_EXCL also refuses an existing symlink; overwrite must be explicit.
    if target.exists() or target.is_symlink():
        if not overwrite:raise FileExistsError(str(target))
        if target.is_symlink():raise ValueError('Refusing to overwrite a symlink.')
        import datetime
        shutil.copy2(target,target.with_name(target.name+'.bak-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S-%f')))
    temporary=None
    try:
        with tempfile.NamedTemporaryFile('w',dir=root,delete=False) as output:
            temporary=Path(output.name);output.write(content)
        temporary.chmod(0o755)
        if overwrite:temporary.replace(target)
        else:os.link(temporary,target);temporary.unlink()
    finally:
        if temporary:temporary.unlink(missing_ok=True)
    return target


def desktop(command=None):
    app=app_from_command(command or input('Binary or full path [telegram]: ').strip() or 'telegram')
    if not resolve(app['argv']):missing(app['name']);return
    app['name']=input('Display name ['+app['name']+']: ').strip() or app['name']
    default_icon=Path(app['argv'][0]).stem.lower()
    icon=input('Icon theme name or full image path ['+default_icon+']: ').strip() or default_icon
    terminal=input('Run inside a terminal? [y/N]: ').strip().lower() in ('y','yes')
    try:path=install_desktop(app,icon,terminal)
    except FileExistsError as error:
        if input(str(error)+' exists. Back up and replace it? [y/N]: ').strip().lower() not in ('y','yes'):return
        path=install_desktop(app,icon,terminal,overwrite=True)
    print('Installed '+str(path)+' (available in Show Applications).')
    if input('Pin it to the GNOME dock? [y/N]: ').strip().lower() in ('y','yes'):
        print('Pinned to dock.' if pin(path.name) else 'Automatic pinning unavailable; open the app and choose Add to Favorites.')


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('action',choices=['configure','list','add','remove','clear','run','desktop'],nargs='?',default='list')
    parser.add_argument('items',nargs='*')
    args=parser.parse_args()
    try:
        if args.action=='configure':configure()
        elif args.action=='run':launch_apps()
        elif args.action=='desktop':desktop((args.items[0] if len(args.items)==1 else shlex.join(args.items)) if args.items else None)
        elif args.action=='list':
            apps=load();print('\n'.join(a['name']+': '+shlex.join(a['argv']) for a in apps) or 'No extra login apps. Run setupapps to opt in.')
        elif args.action=='clear':save([]);print('Extra login apps disabled.')
        elif args.action=='remove':save([a for a in load() if a['name'].lower() not in {v.lower() for v in args.items}])
        elif args.action=='add':
            if not args.items:raise ValueError('Use loginapps add telegram slack, or setupapps for paths/arguments.')
            apps=load()
            for command in args.items:
                app=app_from_command(command);apps=[a for a in apps if a['name'].lower()!=app['name'].lower()]+[app]
            save(apps);print('Saved. dailylogin will open your selected apps.')
    except (EOFError,KeyboardInterrupt):parser.exit(1,'\nCancelled.\n')
    except (OSError,ValueError) as error:parser.exit(1,str(error)+'\n')

if __name__=='__main__':main()
