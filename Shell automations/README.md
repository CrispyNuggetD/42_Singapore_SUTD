# Shell automations

**Forgot to push before leaving on Friday?** Back up saved work when you lock your
screen, then retrieve it over the weekend. You still need to share it with teammates.

## Features and scope

1. ✅ **Private WIP backups on lock** — after setup, snapshots tracked changes,
   deletions and non-ignored new files to your own private GitHub repo.
2. ✅ **Reusable tmux workspace** — `dli` opens/reuses your workspace and prints
   local login-hour reports. Sessions keep running during lock backups.
3. ✅ **Your own final login commands** — a commented Bash starter script runs
   last in `dli`; add whatever you need.
4. ✅ **Choose your extras** — project opening, Git sync, Codex, Chrome, LAN42,
   brightness and automatic updates are off by default.
5. ✅ **Helpers-only download** — skip my curriculum projects and use your own.
6. ✅ **User-level setup** — core installation and backups need no sudo.
   Existing shell content is preserved and backed up.

**It does NOT:**

- ❌ Upload on clone/install: private backups require a separate setup step.
- ❌ Push to your team's `main`, grant teammates access, or capture unsaved buffers
  or nested repo/submodule contents.
- ❌ Change your working branch, staged changes or public `origin` during backups.
- ❌ Export your Codex chats by default or use my private backup destination.
- ❌ Change campus lock/logout rules, close tmux during lock backups, or perform
  a broad `rm -rf` wipe.

**Defaults:** `dli` prints reports, opens a tmux shell, reminds you to configure
backups and runs your initially empty personal script. Disable the reminder/backups
with `privatebackup=FALSE`; disable the final script with `loginitems=FALSE`.

> ⚠️ **Warning**
>
> **Authorship:** ideas by hnah; newer (python) scripts are fully AI-written (“vibe coded”), but I run them on my own 42 Ubuntu + Mac to
> review and test for specific behavior.
>
> Use at your own risk; this is not a security audit. (There's no `rm -rf` however...)
>
> Review files before enabling uploads and keep the destination private.

<details>
<summary><strong>Install: helpers only or full repo, then configure backups</strong></summary>

### Download

Helpers only (Git installed):

```sh
git clone --filter=blob:none --sparse https://github.com/CrispyNuggetD/42_Singapore_SUTD.git 42-shell-helpers
cd 42-shell-helpers
git sparse-checkout set --no-cone '/README.md' '/AGENTS.md' '/*.py' '/Useful .zshrc edits (addition)' '/Shell automations/' '/Study Notes/Codex/' '/Setup .vimrc guide' '/Compiling 42.txt' '/Random scripts/'
```

For the whole repo, omit `--filter=blob:none --sparse` and the `sparse-checkout`
command. Cloning only downloads files.

### Install shell helpers

Use a zsh terminal with Python 3 and tmux installed:

```sh
python3 sync_zshrc.py --check  # optional preview
python3 sync_zshrc.py
source ~/.zshrc
setupzshrc                   # choose YOUR project repo and settings
dli                          # run selected tasks
```

Installs a marked block in `~/.zshrc`, code in `~/.local/share/42-shell/`, mouse
scrolling in `~/.tmux.conf`, choices in `~/.42-shell-settings.zsh`, and a commented
`~/.42-login-items.sh`. Existing configuration is backed up; your script is preserved.
Installing/sourcing the helpers does not automatically run `dli`.

### Enable private lock backups

Create an empty PRIVATE GitHub repo and have working GitHub SSH access:

```sh
setupprivatebackup           # bind YOUR school repo and private SSH URL
privatebackupnow --preview   # inspect included files
privatebackupnow             # test a checkpoint and push
privatebackupstatus          # check setup/service
```

Requires GNOME/Linux, systemd and Python `gi`. Adds a `backup` remote, JSON config,
user systemd service and desktop autostart. Locks and switches away from the desktop
(including other TTYs) trigger snapshots to private `wip/42-school`. Failed uploads
keep their local checkpoint; no-change locks retry the push.

The installer rejects publicly visible repos and asks you to confirm privacy.
Known credential filenames block a checkpoint; this is not a complete secret detector.

Config: `~/.config/42-lock-checkpoint.json`.
Log: `~/.local/state/42-lock-backup/watch.log`. XDG overrides apply.

Edit `~/.42-shell-settings.zsh` to set `privatebackup=FALSE` or `loginitems=FALSE`,
then run `source ~/.zshrc`. Update with `git pull --ff-only`, then
`python3 sync_zshrc.py`.

### Optional-command effects

Project opening runs `make fclean`; Git sync can stage/commit/push; Codex startup
enables full access. `setupclipboard` can invoke sudo for dependencies. The separate
`leaveschool` command closes all tmux sessions after successful syncing; lock backups
do not. A configured private chat-return clone is checked/pulled by `dli`; otherwise
that task is skipped.

</details>

## Optional: my setup and reference

My setup additionally enables helper updates, project clean/open, Codex, LAN42,
my enrolled mailbox and brightness adjustment. Separate local hooks run `dli` once
per desktop login and restore brightness after unlock. My optional chat backups go
to a separate private repo; these settings are not installed for friends.

**Stayon is not allowed at 42SG for meeting attendance requirements. It is off in
my own setup (`startstayon=FALSE`) too.** Campus forced logout still applies.

- [Shared zsh code](../Useful%20.zshrc%20edits%20%28addition%29), [installer](../sync_zshrc.py), [settings wizard](../setup_zshrc.py).
- [Backup installer](../setup_private_backup.py), [snapshot code](../lock_backup.py), [lock watcher](../lock_backup_watch.py).
- [Detailed shell guide](../Study%20Notes/Codex/shell-setup.md), [tmux guide](../Study%20Notes/Codex/tmux-workflow.md).
- [Vim tips](../Setup%20.vimrc%20guide), [compiling notes](../Compiling%2042.txt), [miscellaneous scripts](../Random%20scripts/).

Update with `git pull --ff-only`, then `python3 sync_zshrc.py`. To stop an installed
backup service immediately: `systemctl --user stop 42-lock-backup.service`; also
save `privatebackup=FALSE`. To unload shell helpers, remove their marked `.zshrc`
block or restore its pre-install backup. Configuration files remain on disk.
