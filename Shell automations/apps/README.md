# Optional login apps and dock launchers

These helpers are shared with everyone using this repository's shell setup.
**No extra apps start by default.** Each person chooses their own list; the list
and machine-specific executable paths stay in
`~/.config/42-shell/apps.json` (or `$XDG_CONFIG_HOME/42-shell/apps.json`), outside Git.
No Codex, sudo, or third-party Python packages are needed.

After pulling the helpers clone, install/refresh them:

```zsh
python3 sync_zshrc.py
source ~/.zshrc
setupapps
```

`setupapps` opens a terminal menu to add/remove/list apps. Enter a binary name
such as `telegram`, `slack`, `firefox`, or a full executable path. Paths containing
spaces work. For arguments, use shell-style quoting, for example
`"/sgoinfre/goinfre/Perso/YOUR_LOGIN/apps/My App" --minimized`.
Commands are stored as argument lists and launched without a shell.
Saving an empty list disables extra app startup; existing Chrome and VS Code
settings remain separate.

Shortcuts:

```zsh
loginapps                     # list your selected apps
loginapps add telegram slack  # opt in to both
loginapps remove Telegram     # remove by display name
loginapps clear               # disable all extra login apps
loginapps run                 # open your selected apps now
```

`dailylogin` reads the list on every run, so changes take effect without reloading
other terminals. Already-running apps are kept when their executable can be
identified. Telegram also recognizes a personal `~/.local/bin/telegram-launch`.
Custom launch scripts may need their actual app executable configured for
reliable running-process detection.

If an executable is missing, the helper prints a Google search such as
**install telegram ubuntu** and, in an interactive terminal, asks whether to open
it. It does not install software or run commands copied from search results.

Campus HOME has about **5 GB** of space. Store large portable apps in your own
sgoinfre directory, for example `/sgoinfre/goinfre/Perso/YOUR_LOGIN/apps/`.
Choose the actual executable inside an extracted app directory. Keep small
settings and launchers in HOME. Check your campus's storage rules and whether
that path is available on the computer you are using.

## Create an application menu/dock launcher

```zsh
desktopapp           # asks for a binary; Enter defaults to telegram
desktopapp telegram
desktopapp '/sgoinfre/goinfre/Perso/YOUR_LOGIN/apps/Telegram/Telegram'
```

The terminal prompts for a display name, icon, and whether a terminal is needed.
Press Enter for the default name/icon (for `telegram`, these produce
`telegram.desktop` and icon theme name `telegram`). For a custom icon, enter an
installed icon theme name or the full path to a local image. A default icon name
only displays if your desktop's icon theme provides it; otherwise choose an image.

Launchers are installed in `~/.local/share/applications/` (or
`$XDG_DATA_HOME/applications/`) and appear in Show Applications. The tool asks
before backing up and replacing an existing launcher. It also offers **optional**
GNOME dock pinning, preserving existing favorites and their order. If automatic
pinning is unavailable, open the app, right-click its dock icon, and choose
Add to Favorites. Nothing is installed system-wide.

The launcher uses the configured executable path; keep that app directory in
place. Creating a launcher does not also opt the app into dailylogin.

## How script updates work

`sync_zshrc.py` installs `app_manager.py` alongside the other shared scripts in
`~/.local/share/42-shell/` (or `$ZSHRC_RUNTIME_ROOT`). `setupapps`, `loginapps`, and
`desktopapp` call that installed copy. A later sync refreshes the code and keeps
everyone's personal app list. Helpers-only sparse clones already include root
Python files, so they receive this helper too. LAN42's separate updater handles
LAN42; these app helpers belong to this repository's shell installer.

Tests: `python3 -m unittest discover -s 'Shell automations/apps'`.
