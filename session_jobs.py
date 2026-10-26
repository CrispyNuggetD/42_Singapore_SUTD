#!/usr/bin/env python3
"""Opt-in unattended shell jobs for the existing desktop backup watcher.

Config: ~/.config/42-shell/session-jobs.json. Commands use existing zsh functions;
no sudo, synthetic activity, or changes to the school lock/logout policy.
"""
import json
import os
from pathlib import Path
import subprocess
import time


def solver_running(proc=Path('/proc')):
    for entry in proc.iterdir():
        if not entry.name.isdigit():continue
        try:
            if entry.stat().st_uid!=os.getuid():continue
            args=(entry/'cmdline').read_bytes().split(b'\0')
            if any(a.endswith(b'/run_random_tests.py') or a==b'run_random_tests.py' for a in args):return True
        except (OSError,PermissionError):pass
    return False


def due(idle_seconds,away_seconds,already_left):
    return not already_left and (idle_seconds>=40*60 or away_seconds>=35*60)


class SessionJobs:
    def __init__(self,log,note,clock=time.monotonic):
        self.log,self.note,self.clock=log,note,clock
        self.away_since=None
        self.left=False
        self.next_push=None
        self.push=None
        self.leave=None
        self.was_running=False
        self.leave_retry=0

    def config(self):
        path=Path(os.environ.get('XDG_CONFIG_HOME',str(Path.home()/'.config')))/'42-shell/session-jobs.json'
        try:return json.loads(path.read_text())
        except (OSError,ValueError):return {}

    def launch(self,command):
        self.note('Session job starting: '+command)
        # A separate process session survives tmux closing during leavesch.
        return subprocess.Popen(['/bin/zsh','-ic',command],stdin=subprocess.DEVNULL,stdout=self.log,stderr=self.log,
                                start_new_session=True,env=dict(os.environ,GIT_TERMINAL_PROMPT='0'))

    def tick(self,away,backup_busy=False):
        config=self.config();now=self.clock()
        if away:
            if self.away_since is None:self.away_since=now
        else:self.away_since=None;self.left=False
        for name in ('push','leave'):
            process=getattr(self,name)
            if process is not None and process.poll() is not None:
                self.note('Session '+name+' finished: '+str(process.returncode))
                if name=='leave' and process.returncode:
                    self.left=False
                    self.leave_retry=now+120
                setattr(self,name,None)
        if self.leave is not None:return True
        idle=0
        if config.get('idle_leavesch'):
            try:
                result=subprocess.run(['xprintidle'],capture_output=True,text=True,timeout=3,check=True)
                idle=int(result.stdout)/1000
            except (OSError,ValueError,subprocess.SubprocessError):pass
            away_for=now-self.away_since if self.away_since is not None else 0
            if due(idle,away_for,self.left) and now>=self.leave_retry and not backup_busy and self.push is None:
                self.left=True
                self.leave=self.launch('leavesch')
                return True
            if idle<60 and not away:self.left=False
        running=solver_running() if config.get('solver_gitpushswap') else False
        if running and not self.was_running:
            # Use elapsed runtime so enabling the watcher covers an already
            # running solver. Fall back to a fresh interval if unavailable.
            elapsed=0
            try:
                values=subprocess.check_output(['ps','-u',str(os.getuid()),'-o','etimes=,args='],text=True,timeout=3)
                elapsed=max([int(line.split(None,1)[0]) for line in values.splitlines() if 'python' in line and 'run_random_tests.py' in line] or [0])
            except (OSError,ValueError,subprocess.SubprocessError):pass
            self.next_push=now+max(0,42*60-elapsed)
            self.note('Solver detected; next gitpushswap in '+str(max(0,int(self.next_push-now)))+' seconds (42-minute interval)')
        self.was_running=running
        if running and self.next_push is not None and now>=self.next_push and self.push is None and not backup_busy:
            self.push=self.launch('gitpushswap')
            self.next_push=now+42*60
        if not running:self.next_push=None
        return True
