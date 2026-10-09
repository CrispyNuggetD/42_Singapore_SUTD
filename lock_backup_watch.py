#!/usr/bin/python3
"""Checkpoint once on lock/inactivation; keep desktop jobs running."""
import fcntl
import json
import os
from pathlib import Path
import subprocess
from gi.repository import Gio, GLib
from lock_backup import CONFIG

runtime = Path(os.environ['XDG_RUNTIME_DIR'])
lock = (runtime / '42-lock-backup-watch.lock').open('w')
try:
    fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
except BlockingIOError:
    raise SystemExit(0)
state = Path(os.environ.get('XDG_STATE_HOME', str(Path.home() / '.local/state'))) / '42-lock-backup'
state.mkdir(parents=True, exist_ok=True)
log = (state / 'watch.log').open('a', buffering=1)
def note(message):
    log.write(GLib.DateTime.new_now_local().format('%F %T') + ' ' + message + '\n')

system = Gio.bus_get_sync(Gio.BusType.SYSTEM, None)
bus = Gio.bus_get_sync(Gio.BusType.SESSION, None)
manager = Gio.DBusProxy.new_sync(system, Gio.DBusProxyFlags.NONE, None,
    'org.freedesktop.login1', '/org/freedesktop/login1', 'org.freedesktop.login1.Manager', None)
sessions = manager.call_sync('ListSessions', None, Gio.DBusCallFlags.NONE, -1, None).unpack()[0]
ours = [s for s in sessions if s[1] == os.getuid() and s[3]]
requested = os.environ.get('XDG_SESSION_ID')
session = next((s for s in ours if s[0] == requested), ours[0] if ours else None)
if session is None:
    raise SystemExit('No desktop session found')
proxy = Gio.DBusProxy.new_sync(system, Gio.DBusProxyFlags.NONE, None,
    'org.freedesktop.login1', session[4], 'org.freedesktop.login1.Session', None)
active = proxy.get_cached_property('Active').unpack()
locked_property = proxy.get_cached_property('LockedHint')
locked = locked_property.unpack() if locked_property else False
screen_locked = False
fired = False
child = None
queued = False

def enabled():
    try:
        settings = Path(os.environ.get('ZSHRC_SETTINGS_FILE', str(Path.home() / '.42-shell-settings.zsh')))
        from setup_zshrc import read_settings
        if read_settings(settings).get('privatebackup', 'TRUE').upper() not in ('TRUE', '1'):
            return False
        return json.loads(CONFIG.read_text()).get('enabled', False)
    except (OSError, ValueError):
        return False

def finished(pid, status):
    global child, queued
    result = os.waitstatus_to_exitcode(status)
    note('Backup process finished: ' + str(result))
    if child is not None:
        child.returncode = result
    child = None
    if queued:
        queued = False
        launch()

def launch():
    global child, queued
    if not enabled():
        note('Automatic backup disabled')
        return
    if child is not None:
        queued = True
        note('Backup running; queued next checkpoint')
        return
    note('Starting school snapshot and optional private chat backup')
    child = subprocess.Popen(['/usr/bin/python3', str(Path(__file__).with_name('lock_backup.py'))], stdout=log, stderr=log)
    GLib.child_watch_add(GLib.PRIORITY_DEFAULT, child.pid, finished)

def evaluate():
    global fired
    away = not active or locked or screen_locked
    if away and not fired:
        fired = True
        launch()
    elif not away:
        fired = False
        note('Desktop returned; jobs left running')

def properties_changed(connection, sender, path, interface, signal, parameters, data):
    global active, locked
    iface, changed, invalidated = parameters.unpack()
    if iface != 'org.freedesktop.login1.Session':
        return
    note('Session properties: ' + repr(changed))
    active = changed.get('Active', active)
    locked = changed.get('LockedHint', locked)
    evaluate()

def screensaver_changed(connection, sender, path, interface, signal, parameters, data):
    global screen_locked
    screen_locked = parameters.unpack()[0]
    note('ScreenSaver ActiveChanged: ' + str(screen_locked))
    evaluate()

system.signal_subscribe('org.freedesktop.login1', 'org.freedesktop.DBus.Properties',
    'PropertiesChanged', session[4], None, Gio.DBusSignalFlags.NONE, properties_changed, None)
bus.signal_subscribe('org.gnome.ScreenSaver', 'org.gnome.ScreenSaver',
    'ActiveChanged', '/org/gnome/ScreenSaver', None, Gio.DBusSignalFlags.NONE, screensaver_changed, None)
note('Watching desktop session ' + session[0])
evaluate()
GLib.MainLoop().run()
