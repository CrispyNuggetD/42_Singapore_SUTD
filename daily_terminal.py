#!/usr/bin/env python3
"""Named tmux windows for dailylogin and standalone terminal helpers."""
import argparse
from contextlib import contextmanager
import fcntl
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import time

# An explicit socket path is useful for isolated tests or a separate workspace.
def tmux(*args, check=True):
    command = ["tmux"]
    if os.environ.get("DAILY_TMUX_SOCKET"):
        command += ["-S", os.environ["DAILY_TMUX_SOCKET"]]
    return subprocess.run(command + list(args), check=check, text=True,
                          stdout=subprocess.PIPE, stderr=subprocess.PIPE)

def session_name():
    name = os.environ.get("DAILY_TMUX_SESSION", "daily")
    if not re.fullmatch(r"[A-Za-z0-9_-]+", name):
        raise ValueError("DAILY_TMUX_SESSION must use letters, digits, underscores or hyphens.")
    return name

@contextmanager
def launch_lock():
    directory = Path(os.environ.get("XDG_STATE_HOME", str(Path.home() / ".local/state"))) / "daily-tmux"
    directory.mkdir(parents=True, exist_ok=True, mode=0o700)
    with (directory / "launch.lock").open("a") as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        yield

def exists(session):
    return tmux("has-session", "-t", "=" + session, check=False).returncode == 0

def find_window(session, role):
    if not exists(session):
        return None
    rows = tmux("list-windows", "-t", "=" + session, "-F",
                "#{window_id}\t#{@daily_role}").stdout.splitlines()
    return next((row.split("\t")[0] for row in rows
                 if row.partition("\t")[2] == role), None)

def open_window(session, role, cwd, command):
    if not re.fullmatch(r"[A-Za-z0-9_-]+", role):
        raise ValueError("Window role must use letters, digits, underscores or hyphens.")
    if not Path(cwd).is_dir():
        raise ValueError("Working directory missing: " + cwd)
    with launch_lock():
        window = find_window(session, role)
        if window:
            states = tmux("list-panes", "-t", window, "-F", "#{pane_dead}").stdout.splitlines()
            if any(state == "0" for state in states):
                print("Already open: " + session + " / " + role)
                return window
        else:
            # Start a waiting shell, set retention first, then replace our shell.
            # This also retains errors from commands that fail immediately.
            if exists(session):
                window = tmux("new-window", "-d", "-P", "-F", "#{window_id}",
                              "-t", session + ":", "-n", role, "-c", cwd,
                              "/bin/sh").stdout.strip()
            else:
                window = tmux("new-session", "-d", "-P", "-F", "#{window_id}",
                              "-s", session, "-n", role, "-c", cwd,
                              "/bin/sh").stdout.strip()
            tmux("set-option", "-w", "-t", window, "@daily_role", role)
        tmux("set-option", "-w", "-t", window, "remain-on-exit", "on")
        tmux("set-option", "-w", "-t", window, "automatic-rename", "off")
        # Refresh desktop connection details rather than using the server's old ones.
        invocation = ["env"]
        for key in ("DAILY_TMUX_BATCH", "GNOME_TERMINAL_SCREEN", "GNOME_TERMINAL_SERVICE",
                    "GIO_LAUNCHED_DESKTOP_FILE", "GIO_LAUNCHED_DESKTOP_FILE_PID"):
            invocation += ["-u", key]
        for key in ("PATH", "DISPLAY", "WAYLAND_DISPLAY", "XAUTHORITY",
                    "DBUS_SESSION_BUS_ADDRESS", "SSH_AUTH_SOCK", "DAILY_TMUX_HELPER",
                    "DAILY_TMUX_SESSION", "MAIN_REPO_ROOT", "MAC_REMOTE_ROOT",
                    "STAYON_ROOT", "LAN42_CLONE"):
            if key in os.environ:
                invocation.append(key + "=" + os.environ[key])
        tmux("respawn-window", "-k", "-t", window, "-c", cwd, *invocation, *command)
        print("Opened: " + session + " / " + role)
        return window

def attached(session):
    rows = tmux("list-sessions", "-F", "#{session_name}\t#{session_attached}", check=False).stdout.splitlines()
    for row in rows:
        name, _, count = row.partition("\t")
        if name == session:
            return int(count)
    return 0

def show(session, window=None):
    if not exists(session):
        print("No daily windows yet. Run start_codex, start_lan42, or dailylogin.")
        return
    # A standalone launch from tmux switches its existing client; never nest tmux.
    if os.environ.get("TMUX"):
        tmux("switch-client", "-t", "=" + session)
        if window:
            tmux("select-window", "-t", window)
        return
    with launch_lock():
        if attached(session):
            print("Already visible in a terminal. Use Prefix → w to choose a window.")
            return
        attach = ["tmux"]
        if os.environ.get("DAILY_TMUX_SOCKET"):
            attach += ["-S", os.environ["DAILY_TMUX_SOCKET"]]
        attach += ["attach-session", "-t", "=" + session]
        if window:
            tmux("select-window", "-t", window)
        if not (os.environ.get("DISPLAY") or os.environ.get("WAYLAND_DISPLAY")):
            print("Session is running. Reconnect: tmux attach -t " + session)
            return
        if shutil.which("gnome-terminal"):
            desktop = ["gnome-terminal", "--window", "--title=Daily workspace", "--", *attach]
        elif shutil.which("xterm"):
            desktop = ["xterm", "-T", "Daily workspace", "-e", *attach]
        else:
            print("No desktop terminal found. Reconnect: tmux attach -t " + session)
            return
        env = dict(os.environ)
        for key in ("DAILY_TMUX_BATCH", "GNOME_TERMINAL_SCREEN", "GNOME_TERMINAL_SERVICE",
                    "GIO_LAUNCHED_DESKTOP_FILE", "GIO_LAUNCHED_DESKTOP_FILE_PID"):
            env.pop(key, None)
        # Hold the launch lock until attached, preventing simultaneous GUI requests.
        process = subprocess.Popen(desktop, env=env, start_new_session=True,
                                   stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL,
                                   stderr=subprocess.DEVNULL)
        for _ in range(40):
            if attached(session):
                return
            if process.poll() not in (None, 0):
                raise RuntimeError("Terminal launch failed. Reconnect: tmux attach -t " + session)
            time.sleep(0.1)
        raise RuntimeError("Terminal did not attach. Session is running; use: tmux attach -t " + session)

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    actions = parser.add_subparsers(dest="action", required=True)
    opener = actions.add_parser("open")
    opener.add_argument("role")
    opener.add_argument("--cwd", default=os.getcwd())
    opener.add_argument("command", nargs=argparse.REMAINDER)
    actions.add_parser("show")
    actions.add_parser("list")
    args = parser.parse_args()
    if not shutil.which("tmux"):
        raise RuntimeError("tmux is required. Install it before launching daily terminals.")
    session = session_name()
    if args.action == "list":
        if exists(session):
            print(tmux("list-windows", "-t", "=" + session, "-F",
                       "#{window_index}: #{window_name} | panes: #{window_panes} | active: #{window_active}").stdout, end="")
        else:
            print("No daily windows yet.")
    elif args.action == "show":
        show(session)
    else:
        command = args.command
        if command[:1] == ["--"]:
            command = command[1:]
        if not command:
            parser.error("open requires a command after --")
        window = open_window(session, args.role, args.cwd, command)
        if os.environ.get("DAILY_TMUX_BATCH") != "1":
            show(session, window)

if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError, RuntimeError, subprocess.CalledProcessError) as error:
        detail = error.stderr.strip() if isinstance(error, subprocess.CalledProcessError) else str(error)
        sys.exit("daily tmux: " + detail)

