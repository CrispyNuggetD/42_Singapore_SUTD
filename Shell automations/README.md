# Shell automations

Reusable 42 Ubuntu shell helpers for your own repositories, with public setup
instructions and defaults below. The bottom section describes the fuller
workflow **I personally use**; those personal settings are not copied to you.

## Public setup and defaults

**Only want an icon that Git pushes and locks?**
[Install standalone Go Break](gobreak/README.md). It works with or without Codex
and does not change your `.zshrc`. It commits and pushes your chosen repos'
current upstream branches, separately from the private backup setup below.
The full shell installer includes the same feature and keeps both copies
updated on each sync; existing standalone selections are preserved.

**Forgot to push before leaving on Friday?** Back up saved work when you lock your
screen, then retrieve it over the weekend. You still need to share it with teammates.

## Optional login apps and desktop launchers

Run `setupapps` to choose apps opened by `dailylogin` (empty/off by default),
`loginapps` to view the list, and `desktopapp` to create a user-level launcher
with optional dock pinning. Telegram, Slack, and arbitrary executable paths are
supported. [Usage, sgoinfre storage tips, and script syncing](apps/README.md).

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

**Private lock-backup scope:**

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

The full installer also installs and pins the Go Break icon; `--no-pin` skips
pinning. Clicking the full-shell icon scans `~/Documents` by default, respecting
only your configured exclusions. Configure `GOBREAK_REPOS_ROOT`, `GOBREAK_INCLUDE_REPOS`
and `GOBREAK_EXCLUDE_REPOS` for your repos, or use the standalone installer for
one repo by default. Unlike the private lock-backup service, Go Break commits
and pushes the selected repos' current upstream branches. Installation itself
does not commit or push.

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
git sparse-checkout set --no-cone '/README.md' '/AGENTS.md' '/sync_zshrc.py' '/install_gobreak.py' '/tools/' '/Useful .zshrc edits (addition)' '/Shell automations/' '/Study Notes/Codex/' '/docs/vim-setup.md' '/Piscine Preparation/early-c-exercises/' '/Random scripts/'
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
Also installs the shared Go Break helpers and dock icon; an existing standalone
repo selection is preserved. Use `python3 sync_zshrc.py --no-pin` to skip pinning.
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
enables full access. `setupclipboard` checks the school-provided clipboard backend without sudo. The separate
`leavesch` command closes all tmux sessions after successful syncing; lock backups
do not. A configured private chat-return clone is checked/pulled by `dli`; otherwise
that task is skipped.

</details>

## Full automation I personally use

My setup additionally enables helper updates, project clean/open, Codex, LAN42,
my enrolled mailbox and brightness adjustment. Separate local hooks run `dli` once
per desktop login and restore brightness after unlock. My optional chat backups go
to a separate private repo; these settings are not installed for friends.

My Go Break scans `~/Documents` and keeps the mailbox, submission
`Documents/Projects`, and `Documents/Darren's 42 Repo` exclusions. Includes and
extra exclusions live in my personal `~/.42-shell-settings.zsh`. The implementation
is shared with standalone Go Break; only the configured scope and full-workflow
steps differ.

[My main public 42 repository](https://github.com/CrispyNuggetD/42_Singapore_SUTD)
and [42SG Campus LAN](https://github.com/CrispyNuggetD/42sg-campus-lan) are separate
clones. Questions or feedback: **hnah (Christopher Nah), 42SG Cohort 4**, on
**42SG Slack**.

**Stayon is not allowed at 42SG for meeting attendance requirements. It is off in
my own setup (`startstayon=FALSE`) too.** Campus forced logout still applies.

- [Shared zsh code](../Useful%20.zshrc%20edits%20%28addition%29), [installer](../sync_zshrc.py), [settings wizard](../tools/setup_zshrc.py).
- [Backup installer](../setup_private_backup.py), [snapshot code](../lock_backup.py), [lock watcher](../lock_backup_watch.py).
- [Detailed shell guide](../Study%20Notes/Codex/shell-setup.md), [tmux guide](../Study%20Notes/Codex/tmux-workflow.md).
- [Vim tips](../Setup%20.vimrc%20guide), [compiling notes](../Compiling%2042.txt), [miscellaneous scripts](../Random%20scripts/).

Update with `git pull --ff-only`, then `python3 sync_zshrc.py`. To stop an installed
backup service immediately: `systemctl --user stop 42-lock-backup.service`; also
save `privatebackup=FALSE`. To unload shell helpers, remove their marked `.zshrc`
block or restore its pre-install backup. Configuration files remain on disk.

The main checkpoint stages project paths explicitly after the side-push. Debug
reports and non-project files written during plotting or pushing remain pending
for the next side-push, even if another process stages them. Codex receives only
the staged project diff; a debug-only update does not create a main WIP commit.
Renames involving project code keep both paths together in the main commit.

### Copy a file or folder path

`copypwd` copies the current directory. `copypwd foo.txt` copies the absolute
path of that file; `copypwd folder/` works for folders too. Tab completes files
and directories, including names containing spaces. For example, type
`copypwd fo`, press Tab to complete `foo.txt`, then Enter to copy its path.
Missing paths report an error and leave the clipboard alone.

### Break versus end of day

`gobreak` and `leavesch` share timestamped checkpoint messages and optional Codex
summary bodies: `WIP: checkpoint …` versus `EOD: checkpoint …`. Saved
`--no-codex`/`GOBREAK_CODEX_CONTEXT=FALSE` preferences apply to both. Full-shell
main-repo side commits are summarized separately from project commits; private
chat exports never enter the summary prompt. Both attempt `ft_lock` even if a
save fails, Python cannot start, or the command is interrupted. `gobreak` keeps
tmux running; `leavesch` closes it only after successful saving and pushing.


### Leave and backup recovery

`leavesch` attempts every backup, repository push, normal app-quit request and
final save even if an earlier step fails. Any failure keeps tmux and terminal
windows open for review; screen locking is still attempted last. Chrome uses
normal desktop close requests on Linux and normal Quit on macOS, without Unix
signals or force-kill escalation. Unsaved-work prompts require local attention.

Private lock backups compress files larger than 95 MiB losslessly as
`<original-name>.42-backup.gz`, keeping the source file unchanged. Restore one
with `gzip -dc -- file.42-backup.gz > restored-file`. Files still too large after
compression produce an explicit error. If a previous unpublished backup history
contains GitHub-rejected blobs, retain that history and select a fresh backup
branch in the private checkpoint configuration; ordinary retries cannot remove
oversized blobs from older commits. Never force-push or delete the old history
as an automatic repair.

Owner-authorized Codex backups include plain `.jsonl` and compressed
`.jsonl.zst` rollouts, storing only decompressed transcript JSONL files in the
separate private repository's `42/codex_cache`. A chat still changing during
copy is retried on the next backup. `importhome` fetches the upstream handoff
and registers separate resumable copies without pulling over local untracked
chat backups, changing the private working branch, or touching unrelated work.

The checkpoint configuration may explicitly list `extra_files` (relative paths)
for ignored files that must still be backed up. They pass the same credential,
path and size checks; ignore rules in the source repo remain unchanged.
Owner-configured `settings_backup_private` and `settings_backup_remote` add an
independent `wip/42-lan42-settings` snapshot containing only allowlisted portable
preferences under `42/lan42` in the separate private repo. School, chat and
settings jobs each run even if an earlier job fails. Backup success covers only
the configured selection, never unsaved buffers, every ignored file, nested repo
contents or credentials. Report skipped/failed scope explicitly when checking it.
