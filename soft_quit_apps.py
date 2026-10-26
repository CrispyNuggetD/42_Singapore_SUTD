#!/usr/bin/env python3
"""Normal desktop Quit requests for leavesch; no force kill or discard clicks."""
import argparse
import ctypes as C
from dataclasses import dataclass, field
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import time

TERMINALS = {'gnome-terminal', 'gnome-terminal-server', 'org.gnome.terminal', 'xterm',
             'uxterm', 'konsole', 'kitty', 'alacritty', 'terminator', 'tilix',
             'xfce4-terminal', 'org.wezfurlong.wezterm', 'com.apple.terminal',
             'com.googlecode.iterm2', 'com.mitchellh.ghostty'}
SYSTEM_APPS = {'com.apple.finder', 'com.apple.dock', 'com.apple.systemuiserver',
               'com.apple.loginwindow', 'gnome-shell', 'gjs', 'plasmashell',
               'com.rastersoft.ding', 'desktop_window'}


def normalized(value):
    return value.strip().casefold()


def keep_names():
    names = {normalized(name) for name in os.environ.get('LEAVESCH_KEEP_APPS', '').split(',') if name.strip()}
    if os.environ.get('closechromeonleave', 'TRUE').upper() in ('FALSE', '0', 'NO', 'OFF'):
        names.update(('google-chrome', 'chromium', 'chromium-browser', 'com.google.chrome'))
    return names


@dataclass
class App:
    pid: int
    name: str
    windows: list = field(default_factory=list)
    quit_action: object = None
    aliases: set = field(default_factory=set)
    token: str = ''

    def matches(self, names):
        return bool({normalized(self.name), *(normalized(a) for a in self.aliases)} & names)


class X11:
    """Small ICCCM client; only sends WM_DELETE_WINDOW when explicitly supported."""
    class Data(C.Union):
        _fields_ = [('b', C.c_char * 20), ('s', C.c_short * 10), ('l', C.c_long * 5)]

    class Event(C.Union):
        _fields_ = [('pad', C.c_long * 24)]

    def __init__(self):
        class Client(C.Structure):
            _fields_ = [('type', C.c_int), ('serial', C.c_ulong), ('send_event', C.c_int),
                        ('display', C.c_void_p), ('window', C.c_ulong), ('message_type', C.c_ulong),
                        ('format', C.c_int), ('data', self.Data)]
        self.Client = Client
        self.lib = C.CDLL('libX11.so.6')
        signatures = {
            'XOpenDisplay': ([C.c_char_p], C.c_void_p),
            'XDefaultRootWindow': ([C.c_void_p], C.c_ulong),
            'XInternAtom': ([C.c_void_p, C.c_char_p, C.c_int], C.c_ulong),
            'XGetWindowProperty': ([C.c_void_p, C.c_ulong, C.c_ulong, C.c_long, C.c_long,
                                   C.c_int, C.c_ulong, C.POINTER(C.c_ulong), C.POINTER(C.c_int),
                                   C.POINTER(C.c_ulong), C.POINTER(C.c_ulong), C.POINTER(C.c_void_p)], C.c_int),
            'XFree': ([C.c_void_p], C.c_int), 'XFlush': ([C.c_void_p], C.c_int),
            'XCloseDisplay': ([C.c_void_p], C.c_int),
            'XSendEvent': ([C.c_void_p, C.c_ulong, C.c_int, C.c_long, C.c_void_p], C.c_int),
        }
        for name, (args, result) in signatures.items():
            function = getattr(self.lib, name)
            function.argtypes, function.restype = args, result
        self.error_handler = C.CFUNCTYPE(C.c_int, C.c_void_p, C.c_void_p)(lambda *_: 0)
        self.lib.XSetErrorHandler.argtypes = [C.c_void_p]
        self.lib.XSetErrorHandler(self.error_handler)
        self.display = self.lib.XOpenDisplay(None)
        if not self.display:
            raise RuntimeError('No X11 display; this Linux desktop needs X11 for normal window-close requests.')
        self.root = self.lib.XDefaultRootWindow(self.display)

    def atom(self, name):
        return self.lib.XInternAtom(self.display, name.encode(), 0)

    def property(self, window, name):
        actual, fmt, count, remaining, data = C.c_ulong(), C.c_int(), C.c_ulong(), C.c_ulong(), C.c_void_p()
        result = self.lib.XGetWindowProperty(self.display, window, self.atom(name), 0, 65536, 0, 0,
                                            C.byref(actual), C.byref(fmt), C.byref(count), C.byref(remaining), C.byref(data))
        if result or not data.value:
            return b''
        try:
            if fmt.value == 32:
                return list(C.cast(data, C.POINTER(C.c_ulong))[:count.value])
            return C.string_at(data, count.value) if fmt.value == 8 else b''
        finally:
            self.lib.XFree(data)

    def clients(self):
        return self.property(self.root, '_NET_CLIENT_LIST') or []

    def text(self, window, name):
        value = self.property(window, name)
        return value.decode(errors='replace').strip('\0') if isinstance(value, bytes) else ''

    def apps(self):
        apps = {}
        allowed = {self.atom('_NET_WM_WINDOW_TYPE_' + kind) for kind in ('NORMAL', 'DIALOG', 'UTILITY')}
        for window in self.clients():
            kinds = self.property(window, '_NET_WM_WINDOW_TYPE')
            if kinds and not set(kinds) & allowed:
                continue
            pids = self.property(window, '_NET_WM_PID')
            if not pids or not owned(pids[0]):
                continue
            pid = pids[0]
            names = self.text(window, 'WM_CLASS').split('\0')
            name = names[-1] or 'app-' + str(pid)
            app = apps.setdefault(pid, App(pid, name, token=process_token(pid)))
            app.aliases.update(names)
            app.windows.append(window)
        return apps

    def close_window(self, window):
        if self.atom('WM_DELETE_WINDOW') not in (self.property(window, 'WM_PROTOCOLS') or []):
            return False
        event = self.Event()
        client = C.cast(C.byref(event), C.POINTER(self.Client)).contents
        client.type, client.display, client.window = 33, self.display, window
        client.message_type, client.format = self.atom('WM_PROTOCOLS'), 32
        client.data.l[0], client.data.l[1] = self.atom('WM_DELETE_WINDOW'), 0
        sent = self.lib.XSendEvent(self.display, window, 0, 0, C.byref(event))
        self.lib.XFlush(self.display)
        return bool(sent)

    def close(self):
        self.lib.XCloseDisplay(self.display)


def owned(pid):
    try:
        return Path('/proc', str(pid)).stat().st_uid == os.getuid()
    except OSError:
        return False


def process_token(pid):
    try:
        fields = Path('/proc', str(pid), 'stat').read_text().rpartition(')')[2].split()
        return fields[19] if fields[0] != 'Z' else ''
    except (OSError, IndexError):
        return ''


def quit_menu_item(node):
    """Select an enabled Quit menu command, never a dialog/discard button."""
    ident, props, children = node
    if not props.get('enabled', True) or not props.get('visible', True):
        return None
    label = re.sub(r'[_&]', '', props.get('label', '')).strip().casefold()
    if (props.get('enabled', True) and props.get('visible', True)
            and re.fullmatch(r'(quit|exit)( [\w .+-]+)?', label)
            and not re.search(r'\b(force|discard|without|save|lose|kill)\b', label)):
        return ident
    for child in children:
        found = quit_menu_item(child)
        if found is not None:
            return found
    return None


class NativeActions:
    def __init__(self):
        from gi.repository import Gio, GLib
        self.Gio, self.GLib = Gio, GLib
        self.bus = Gio.bus_get_sync(Gio.BusType.SESSION, None)

    def call(self, dest, path, interface, method, signature=None, args=None):
        params = self.GLib.Variant(signature, args) if signature else None
        return self.bus.call_sync(dest, path, interface, method, params, None,
                                  self.Gio.DBusCallFlags.NO_AUTO_START, 1500, None).unpack()

    def prop(self, dest, path, interface, name):
        return self.call(dest, path, 'org.freedesktop.DBus.Properties', 'Get', '(ss)', (interface, name))[0]

    def trays(self, apps):
        try:
            items = self.prop('org.kde.StatusNotifierWatcher', '/StatusNotifierWatcher',
                              'org.kde.StatusNotifierWatcher', 'RegisteredStatusNotifierItems')
        except Exception:
            return
        for item in items:
            try:
                dest, sep, suffix = item.partition('/')
                path = '/' + suffix if sep else '/StatusNotifierItem'
                pid = self.call('org.freedesktop.DBus', '/org/freedesktop/DBus',
                                'org.freedesktop.DBus', 'GetConnectionUnixProcessID', '(s)', (dest,))[0]
                if not owned(pid):
                    continue
                name = self.prop(dest, path, 'org.kde.StatusNotifierItem', 'Id')
                menu = self.prop(dest, path, 'org.kde.StatusNotifierItem', 'Menu')
                layout = self.call(dest, menu, 'com.canonical.dbusmenu', 'GetLayout',
                                   '(iias)', (0, -1, ['label', 'enabled', 'visible']))[1]
                ident = quit_menu_item(layout)
                if ident is not None:
                    app = apps.setdefault(pid, App(pid, name, token=process_token(pid)))
                    app.aliases.add(name)
                    app.quit_action = lambda d=dest, m=menu, i=ident: self.call(
                        d, m, 'com.canonical.dbusmenu', 'Event', '(isvu)',
                        (i, 'clicked', self.GLib.Variant('i', 0), 0))
            except Exception:
                continue

    def gtk_quit(self, x11, app):
        for window in app.windows:
            dest = x11.text(window, '_GTK_UNIQUE_BUS_NAME') or x11.text(window, '_GTK_APPLICATION_ID')
            path = x11.text(window, '_GTK_APPLICATION_OBJECT_PATH')
            if not dest or not path:
                continue
            try:
                actions = self.call(dest, path, 'org.gtk.Actions', 'DescribeAll')[0]
                for name in ('quit', 'app.quit'):
                    if name in actions and actions[name][0] and not actions[name][1]:
                        app.quit_action = lambda d=dest, p=path, n=name: self.call(
                            d, p, 'org.gtk.Actions', 'Activate', '(sava{sv})', (n, [], {}))
                        return
            except Exception:
                continue


def selected(apps, terminals=False):
    return [app for app in apps if not app.matches(SYSTEM_APPS | keep_names())
            and app.matches(TERMINALS) == terminals]


def linux_quit(check=False, terminals=False, timeout=30):
    if os.environ.get('XDG_SESSION_TYPE') == 'wayland':
        raise RuntimeError('App-wide soft quit currently requires an X11 Linux session; no apps were closed.')
    x11 = X11()
    try:
        apps = x11.apps()
        try:
            native = NativeActions()
            native.trays(apps)
            for app in apps.values():
                if app.quit_action is None:
                    native.gtk_quit(x11, app)
        except (ImportError, ValueError, RuntimeError) as error:
            raise RuntimeError('Native app quit discovery needs python3-gi and the session bus; no apps were closed.') from error
        targets = selected(apps.values(), terminals)
        for app in targets:
            method = 'Quit' if app.quit_action else 'close windows'
            print(f'{"Would request" if check else "Requesting"} {method}: {app.name}', flush=True)
        if check:
            return
        errors = []
        pending = []
        for app in targets:
            try:
                if not app.token or process_token(app.pid) != app.token:
                    continue
                if app.quit_action:
                    app.quit_action()
                elif not all([x11.close_window(window) for window in app.windows]):
                    errors.append(app.name + ': no supported normal close action')
                pending.append(app)
            except Exception:
                errors.append(app.name + ': quit request could not be delivered')
        deadline = time.monotonic() + timeout
        while pending and time.monotonic() < deadline:
            clients = set(x11.clients())
            pending = [app for app in pending if process_token(app.pid) == app.token
                       and (app.quit_action is not None or clients.intersection(app.windows))]
            if pending:
                time.sleep(0.2)
        errors += [app.name + ': still open; check its prompts locally' for app in pending]
        if errors:
            raise RuntimeError('; '.join(errors))
    finally:
        x11.close()


MAC_SCRIPT = r'''
ObjC.import('AppKit');
function run(argv) {
    const check = argv[0] === 'check', terminals = argv[1] === 'terminals';
    const keep = JSON.parse(argv[2]), terminalIds = JSON.parse(argv[3]), system = JSON.parse(argv[4]);
    const apps = $.NSWorkspace.sharedWorkspace.runningApplications;
    const pending = [], planned = [];
    for (let i = 0; i < apps.count; i++) {
        const app = apps.objectAtIndex(i);
        const id = ObjC.unwrap(app.bundleIdentifier) || '';
        const name = ObjC.unwrap(app.localizedName) || id;
        if (system.includes(id.toLowerCase()) || keep.includes(id.toLowerCase()) || keep.includes(name.toLowerCase())) continue;
        const isTerminal = terminalIds.includes(id.toLowerCase());
        if (isTerminal !== terminals || (!isTerminal && app.activationPolicy !== 0)) continue;
        planned.push(name);
        if (!check) { app.terminate; pending.push({app: app, name: name}); }
    }
    if (check) return JSON.stringify({planned: planned, pending: []});
    const deadline = Date.now() + Number(argv[5]) * 1000;
    while (pending.some(p => !p.app.terminated) && Date.now() < deadline) delay(0.2);
    return JSON.stringify({planned: planned, pending: pending.filter(p => !p.app.terminated).map(p => p.name)});
}
'''


def mac_quit(check=False, terminals=False, timeout=30):
    result = subprocess.run(['osascript', '-l', 'JavaScript', '-e', MAC_SCRIPT,
                             'check' if check else 'quit', 'terminals' if terminals else 'apps',
                             json.dumps(sorted(keep_names())), json.dumps(sorted(TERMINALS)),
                             json.dumps(sorted(SYSTEM_APPS)), str(timeout)],
                            text=True, capture_output=True, check=True, timeout=timeout + 10)
    report = json.loads(result.stdout)
    for name in report['planned']:
        print(f'{"Would request Quit" if check else "Requested Quit"}: {name}')
    if report['pending']:
        raise RuntimeError('Still open; check prompts locally: ' + ', '.join(report['pending']))


def schedule_terminals():
    directory = Path(os.environ.get('XDG_STATE_HOME', str(Path.home() / '.local/state'))) / '42-shell'
    directory.mkdir(parents=True, exist_ok=True, mode=0o700)
    path = directory / 'terminal-quit.log'
    descriptor = os.open(path, os.O_CREAT | os.O_WRONLY | os.O_APPEND, 0o600)
    with os.fdopen(descriptor, 'a') as output:
        subprocess.Popen([sys.executable, str(Path(__file__).resolve()), '--terminals-only', '--delay', '1'],
                         stdin=subprocess.DEVNULL, stdout=output, stderr=output, start_new_session=True)
    print('Terminal windows will receive normal close requests last.')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true', help='List requests without closing anything')
    parser.add_argument('--terminals-only', action='store_true')
    parser.add_argument('--schedule-terminals', action='store_true')
    parser.add_argument('--delay', type=float, default=0)
    args = parser.parse_args()
    try:
        if args.schedule_terminals:
            if args.check:
                print('Would schedule normal terminal close requests last.')
            else:
                schedule_terminals()
        else:
            if args.delay > 0 and not args.check:
                time.sleep(args.delay)
            action = mac_quit if sys.platform == 'darwin' else linux_quit
            action(check=args.check, terminals=args.terminals_only)
    except Exception as error:
        print('Soft quit needs attention: ' + str(error), file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
