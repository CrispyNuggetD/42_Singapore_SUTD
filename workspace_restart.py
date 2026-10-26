"""Cross-platform shutdown of this user's terminal workspace and managed LAN node."""
import json
import os
from pathlib import Path
import re
import signal
import shutil
import subprocess
import sys
import time


def tmux_command():
    return ['tmux', *(['-S', os.environ['DAILY_TMUX_SOCKET']] if os.environ.get('DAILY_TMUX_SOCKET') else [])]


def processes():
    session = 'sess' if sys.platform == 'darwin' else 'sid'
    result = subprocess.run(['ps', '-u', str(os.getuid()), '-o', f'pid=,ppid=,{session}=,tty=,stat=,comm='],
                            check=True, capture_output=True, text=True)
    rows=[]
    for line in result.stdout.splitlines():
        fields=line.split(None,5)
        if len(fields)!=6:continue
        pid,parent,sid,tty,state,command=fields
        try:sid=int(sid)
        except ValueError:sid=0
        rows.append(dict(pid=int(pid),ppid=int(parent),sid=sid,tty=tty,state=state,
                         command=Path(command).name))
    return rows


def identity(pid):
    try:
        if sys.platform=='darwin':
            result=subprocess.run(['ps','-p',str(pid),'-o','uid=,lstart=,comm='],check=True,capture_output=True,text=True)
            fields=result.stdout.split(None,6)
            if len(fields)!=7 or int(fields[0])!=os.getuid():return None
            return (' '.join(fields[1:6]),fields[6].strip())
        directory=Path('/proc')/str(pid)
        if directory.stat().st_uid!=os.getuid():return None
        fields=(directory/'stat').read_text().rpartition(')')[2].split()
        if fields[0]=='Z':return None
        return (fields[19],str((directory/'exe').resolve()))
    except (OSError,ValueError,IndexError,subprocess.SubprocessError):return None


def managed_nodes():
    root=Path(os.environ.get('XDG_STATE_HOME',str(Path.home()/'.local/state')))/'42sg-campus-lan'
    found=[]
    for file in root.glob('server-*.pid'):
        try:
            pid=int(file.read_text().strip());token=identity(pid)
            if not token:continue
            if sys.platform=='darwin':
                args=subprocess.run(['ps','-p',str(pid),'-o','args='],check=True,capture_output=True,text=True).stdout
            else:args=(Path('/proc')/str(pid)/'cmdline').read_bytes().replace(b'\0',b' ').decode(errors='replace')
            if re.search(r'(?:^|\s)-m\s+campus_lan\s+server(?:\s|$)',args) and '--managed' in args.split():
                found.append((pid,token))
        except (OSError,ValueError,subprocess.SubprocessError):continue
    return found


def pane_processes():
    try:
        result=subprocess.run(tmux_command()+['list-panes','-a','-F','#{pane_pid}\t#{pane_tty}'],capture_output=True,text=True)
    except FileNotFoundError:return [],set()
    roots=[];ttys=set()
    if result.returncode==0:
        for line in result.stdout.splitlines():
            pid,_,tty=line.partition('\t')
            if pid.isdecimal():roots.append(int(pid))
            if tty:ttys.add(tty.removeprefix('/dev/'))
    return roots,ttys


def descendants(rows, roots):
    found=set(roots)
    while True:
        more={row['pid'] for row in rows if row['ppid'] in found}
        if more<=found:return found
        found|=more


def remaining(targets):
    return [(pid,token) for pid,token in targets if identity(pid)==token]


def wait(targets,seconds):
    deadline=time.monotonic()+seconds
    while remaining(targets) and time.monotonic()<deadline:time.sleep(.1)
    return remaining(targets)


def stop(force=False):
    roots,_=pane_processes();rows=processes()
    own=descendants(rows,{os.getpid()})
    jobs=[]
    for pid in descendants(rows,set(roots))-own:
        token=identity(pid)
        if token:jobs.append((pid,token))
    nodes=managed_nodes()
    # A kill-server closes all managed panes and their terminal sessions, not just clients.
    if roots:
        subprocess.run(tmux_command()+['kill-server'],check=True)
    for pid,token in remaining(nodes):
        if identity(pid)!=token:continue
        # asyncio.run handles SIGINT by cancelling and running its cleanup blocks.
        try:os.kill(pid,signal.SIGINT)
        except ProcessLookupError:pass
    targets=list(dict.fromkeys(jobs+nodes))
    pending=wait(targets,3)
    for pid,token in pending:
        if identity(pid)!=token:continue
        try:os.kill(pid,signal.SIGTERM)
        except ProcessLookupError:pass
    pending=wait(pending,3)
    if pending and force:
        for pid,token in pending:
            if identity(pid)!=token:continue
            try:os.kill(pid,signal.SIGKILL)
            except ProcessLookupError:pass
        pending=wait(pending,2)
    if pending:raise RuntimeError('Some workspace processes are still running; startup was not relaunched.')
    print('Old tmux jobs and managed local LAN42 nodes stopped.',flush=True)


MAC_TERMINALS = ['com.apple.Terminal','com.googlecode.iterm2','com.mitchellh.ghostty',
                 'net.kovidgoyal.kitty','org.alacritty','com.github.wez.wezterm']
MAC_DISCOVER = r'''ObjC.import('AppKit');
function run(argv) {
 const ids=JSON.parse(argv[0]), out=[], apps=$.NSWorkspace.sharedWorkspace.runningApplications;
 for(let i=0;i<apps.count;i++) {const a=apps.objectAtIndex(i), id=ObjC.unwrap(a.bundleIdentifier)||'';
 if(ids.includes(id.toLowerCase()))out.push([Number(a.processIdentifier),[String(a.launchDate.timeIntervalSince1970),id]]);}
 return JSON.stringify(out);
}'''


def mac_terminal_targets():
    result=subprocess.run(['osascript','-l','JavaScript','-e',MAC_DISCOVER,json.dumps([v.lower() for v in MAC_TERMINALS])],
                          check=True,capture_output=True,text=True)
    return [(pid,tuple(token)) for pid,token in json.loads(result.stdout)]


def mac_force_terminals(targets):
    script=MAC_DISCOVER.split('function run',1)[0]+r'''
function run(argv) {
 const wanted=JSON.parse(argv[0]), apps=$.NSWorkspace.sharedWorkspace.runningApplications;
 for(let i=0;i<apps.count;i++){const a=apps.objectAtIndex(i), id=ObjC.unwrap(a.bundleIdentifier)||'';
 if(wanted.some(t=>t[0]===Number(a.processIdentifier)&&t[1][0]===String(a.launchDate.timeIntervalSince1970)&&t[1][1]===id))a.forceTerminate;}
 delay(1); return 'ok';
}'''
    subprocess.run(['osascript','-l','JavaScript','-e',script,json.dumps(targets)],check=True,capture_output=True,text=True)
    current={pid:token for pid,token in mac_terminal_targets()}
    return not any(current.get(pid)==token for pid,token in targets)


def launch_login(state,launcher=None):
    environment=dict(os.environ)
    for key in ('TMUX','TMUX_PANE','DAILY_TMUX_BATCH','DAILY_TMUX_DEFER_SHOW','_DAILYLOGIN_REFRESHED',
                'GNOME_TERMINAL_SCREEN','GNOME_TERMINAL_SERVICE','GIO_LAUNCHED_DESKTOP_FILE','GIO_LAUNCHED_DESKTOP_FILE_PID'):
        environment.pop(key,None)
    if launcher:
        subprocess.run([str(launcher)],check=True,env=environment)
        return
    import shlex
    command=['env','LOGIN_STARTUP_SKIP=1','zsh','-ic','dli; exec zsh -i']
    maximize=environment.get('DAILY_TMUX_MAXIMIZE','TRUE').upper() in ('TRUE','1','YES','ON')
    if sys.platform=='darwin':
        script='on run argv\n tell application "Terminal"\n do script (item 1 of argv)\n'
        if maximize:script+='set zoomed of front window to true\n'
        script+='activate\n end tell\nend run'
        desktop=['osascript','-e',script,shlex.join(command)]
    elif shutil.which('gnome-terminal'):
        desktop=['gnome-terminal','--window',*(['--maximize'] if maximize else []),'--',*command]
    elif shutil.which('xterm'):
        desktop=['xterm',*(['-maximized'] if maximize else []),'-e',*command]
    else:raise RuntimeError('No supported desktop terminal launcher found; open a new terminal yourself.')
    (state/'started').touch()
    try:subprocess.run(desktop,check=True,env=environment)
    except Exception:
        (state/'started').unlink(missing_ok=True)
        raise
