# Dailylogin with tmux

## Everyday commands

Reload existing shells once with `source ~/.zshrc`. New shells load the update automatically.

| Command | Result |
| --- | --- |
| `dailylogin` | Runs the existing workflow and shows one tmux workspace. |
| `workterm` | Attaches the current terminal to `daily`; creates it if absent. Inside tmux, switches clients without nesting. |
| `workwindows` | Lists the windows in that workspace. |
| `start_codex` | Starts or reuses the Codex window. |
| `start_lan42` | Starts or reuses the LAN42 lobby window. |
| `start_mailbox` | Starts the mailbox if needed; retains the worker lock check. |
| `stayon` / `stayon stop` / `stayon status` | Keeps existing controls and heartbeat checks; startup uses tmux. |

Existing enable/disable controls still apply. All launch helpers work independently outside tmux.
During dailylogin, terminal launches are collected and the workspace is shown once at the end.
An empty interactive `zsh` window is created after the app windows, in the current project
directory (or the current directory when opening a project is disabled). With Codex,
LAN42, and mailbox enabled, this is the fourth window, `3:zsh`. Later logins reuse
this shell while it is running; exiting it lets the next login restart it.
If already attached elsewhere, another desktop terminal is not opened. Use its Ctrl+B, then w chooser.
A headless launch creates the session and prints a reconnect command.

Ctrl+B, then d hides the workspace without stopping work. Closing the desktop window also leaves it running.
Ctrl+B, then x removes the selected pane after confirmation. Quit programs normally first when possible.
Managed app windows retain stopped output; calling the helper again restarts a fully stopped window.
Ordinary windows created with Ctrl+B, then c use your normal tmux settings.

Chrome startup skips an existing Chrome process belonging to your user.
VS Code uses `--reuse-window`: it may switch an existing editor window to the current project.
Terminals started before this change are not automatically moved into tmux.
Stayon starts only when requested/enabled; tmux does not bypass its remote-disable or heartbeat checks.
Detached work can continue editing files and consuming resources. Sleep pauses local work; reboot ends it.

## Files to include in your SUTD backup

Updated shared source:

- `Useful .zshrc edits (addition)`

New runtime dependency:

- `daily_terminal.py` — keep this alongside the shared source.

Documentation and portable integration patches:

- `Study Notes/Codex/tmux-quick-guide.html` — colored, offline browser guide.
- `Study Notes/Codex/tmux-workflow.md` — this file.
- `Study Notes/Codex/tmux-integrations/lan42-tmux.patch`
- `Study Notes/Codex/tmux-integrations/stayon-tmux.patch`

Also retain the pre-existing `sync_zshrc.py`; the current shell synchronization depends on it.
Files that were already untracked before this change still need your normal Git backup.

Installed in place outside this repository:

- `~/.zshrc` — updated through the existing shared-block synchronizer, preserving local content and the symlink.
- `../42sg-campus-lan/useful-scripts/open_terminal.py` — uses the optional tmux helper for lobby/game terminals.
- `../42_Singapore_SUTD_hnah_private/Scripts and Testers/stayon/stayon.py` — uses the optional tmux helper for startup.

The two small patches above preserve these external integration edits in SUTD without copying the complete private runner.
No commit or push was performed.

## Restore on another campus computer

1. Restore/clone SUTD and the existing LAN42/private repositories. Install tmux, Python 3, and zsh.
2. Apply the integrations from SUTD's repository root (review with `git apply --check` first):

```bash
git -C ../42sg-campus-lan apply --check "$PWD/Study Notes/Codex/tmux-integrations/lan42-tmux.patch"
git -C ../42sg-campus-lan apply "$PWD/Study Notes/Codex/tmux-integrations/lan42-tmux.patch"

git -C ../42_Singapore_SUTD_hnah_private apply --check "$PWD/Study Notes/Codex/tmux-integrations/stayon-tmux.patch"
git -C ../42_Singapore_SUTD_hnah_private apply "$PWD/Study Notes/Codex/tmux-integrations/stayon-tmux.patch"
```

Skip patches already present. If upstream files have changed, review/reconcile the small launch changes instead of forcing them.

3. Sync the shell definitions:

```bash
python3 sync_zshrc.py
source ~/.zshrc
```

Your existing local settings, authentication, mailbox enrollment, and Stayon setup are separate and must already be available.
By default, `DAILY_TMUX_HELPER` points to `$MAIN_REPO_ROOT/daily_terminal.py`, and `DAILY_TMUX_SESSION` is `daily`.
Set overrides outside the shared markers if your paths differ.
