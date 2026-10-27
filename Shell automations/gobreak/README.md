# Go Break: install just the push-and-lock icon

Go Break adds an Ubuntu dock icon that saves your repo's changes in a WIP commit,
pushes to its current upstream branch, and locks your screen.
It works without Codex and leaves your `.zshrc` untouched; an already logged-in
Codex CLI can optionally add a short commit body automatically from the staged
diff. To disable Codex behavior, install with `--no-codex`.

## Quick start

First, open a terminal inside your own project repo. If needed, replace the
path below with your actual repo folder and run this separately:

```sh
cd "/path/to/your/project-repo"
```

Already inside your repo? Skip that step. **Copy and paste this whole install
chunk**:

```sh
gobreak_repo=$(git rev-parse --show-toplevel) &&
git clone --filter=blob:none --sparse https://github.com/CrispyNuggetD/42_Singapore_SUTD.git "$HOME/42-gobreak" &&
git -C "$HOME/42-gobreak" sparse-checkout set --no-cone '/install_gobreak.py' '/tools/' '/Shell automations/gobreak/' &&
python3 "$HOME/42-gobreak/install_gobreak.py" --repo "$gobreak_repo"
```

For example, use `cd "$HOME/Documents/my-project"` to enter your repo.
Check that `cd` succeeded before pasting the install chunk. The chunk uses Git
to find your current repository root; outside a Git repo, it stops before installing.
There is no
repo-selection prompt or setup wizard: the installer prints the selected repo
and what was installed. Run this from your own project repo, not the downloaded
helpers folder. The download uses `~/42-gobreak`; if you already installed it,
use the update instructions below instead.

The installer pins **Go Break (Git push + lock)** to your dock by default.
Add `--no-pin` to the final command to skip pinning. Installation does not
commit, push, or lock; click the icon when ready, or run `git gobreak` in your repo.

Add `--no-codex` to the install command to disable summaries and missing-Codex
messages. This preference persists across updates; `--codex` re-enables it.
The full installer supports `python3 sync_zshrc.py --no-codex` too.
Without that flag, a missing Codex CLI prints a debug message and uses the
existing fallback title; the checkpoint still works.

**Requirements:** a 42 Ubuntu/GNOME desktop with Git, Python 3, Bash, GNOME
Terminal, `flock` and campus `ft_lock`. Your repo needs Git author details,
working remote authentication and a configured upstream branch. Automatic
pinning uses `gsettings`; if unavailable, pin the installed icon manually.
**Optional:** a logged-in Codex CLI for commit bodies; Codex is not installed
by this script. Zsh and tmux are not needed for standalone mode.

## More from hnah

By **hnah (Christopher Nah), 42SG Cohort 4**. Questions or feedback?
Contact **hnah** on **42SG Slack**.
This greeting appears during installation and at the start of every Go Break
run, whether launched from the dock, `git gobreak`, or the main shell command.

- [Full .zshrc automation I personally use](../README.md): daily-login helpers,
  tmux workspace, configurable repository syncing and private lock backups.
- [My main 42 Singapore SUTD repository](https://github.com/CrispyNuggetD/42_Singapore_SUTD):
  school projects, notes and shared scripts.
- [42SG Campus LAN](https://github.com/CrispyNuggetD/42sg-campus-lan):
  my separate campus LAN project; clone it with:

```sh
git clone https://github.com/CrispyNuggetD/42sg-campus-lan.git "$HOME/Documents/42sg-campus-lan"
```

Follow the LAN repo's README for its setup. These are separate installations;
the Go Break quick start downloads only the icon feature and its helpers.

## Installation and repository choices

Already cloned the school repository? Run
`python3 install_gobreak.py --repo /path/to/your/repo` from that clone.
Use your own repository with Git author details, authentication and an upstream
branch already configured. Installation does not push anything.

The Ubuntu application icon is included and pinned to the dock automatically.
Existing favorites and their order are preserved; repeated installs do not add
duplicate pins. To skip pinning, add `--no-pin` to the install command. Shell
syncs remember that preference. The flag does not remove an existing pin.
The full shell installer also accepts `python3 sync_zshrc.py --no-pin`
(or `synczshrc --no-pin` after installation).
If GNOME settings cannot be changed in the current session, installation still
finishes: open **Show Applications**, right-click **Go Break (Git push + lock)**
and choose **Add to Favorites** to pin it manually.
Alternatively run `git gobreak` inside your configured repo. The icon targets
the selected repository; the branch and its upstream are checked at each click.
By default only the selected repo is used. To save extra repo selections:

```sh
python3 install_gobreak.py --repo "$HOME/my-project" \
  --include-repo "$HOME/another-project" \
  --exclude-repo "$HOME/my-project/submission"
```

Repeat `--include-repo` or `--exclude-repo` for several paths. Add
`--scan-root "$HOME/Documents"` to discover repositories beneath a folder.
Excluded folders and all their descendants are skipped. Exclusions win over
includes. A fresh installation selects only your chosen repo root, with no
additional includes or exclusions. There are no special folder-name exclusions:
even `Documents/Darren's 42 Repo` works if that is your own selected repo.
Only exclusions you configure apply, including `DOCUMENTS_PUSH_EXCLUDE` if set.
These selections persist across shell syncs and installer updates.

Each checkpoint stages all saved tracked changes, deletions and non-ignored new
files, commits them, and pushes to the current branch's upstream. It also pushes
existing commits when there are no new changes. Unsaved editor buffers and work
inside nested repos are not captured. Credential-like filenames stop the save
for local review. Locking is attempted even if Git fails; errors stay in the
terminal for review after unlocking. Concurrent clicks run only one checkpoint.

The title is `WIP: checkpoint <timestamp>`, including the local timezone.
The full-shell `leavesch` workflow uses `EOD: checkpoint <timestamp>` with the
same commit and summary implementation. Separate debug/non-project commits use
the same title plus `(debug/non-project)`. With a logged-in Codex CLI,
Codex receives only the staged diff for the paths in that commit to write a
short commit body. Full-shell summaries cover the main school repo, including
its side commits; private chat backups are never sent as summary context. Without Codex, or
if it fails or times out, the commit uses the existing fallback title only.
The installer prints this behavior. Codex is optional; it is never installed or
configured. Set `GOBREAK_CODEX_CONTEXT=FALSE` in the desktop session environment
to disable it, or rerun the installer with `--no-codex` to save that preference.

This standalone feature leaves `.zshrc`, tmux and the daily-login setup untouched.
It uses your selected repositories and does not back up chats. Its three
Python helper files, launcher and icon live in `~/.local/share/42-gobreak/`,
with a desktop entry in `~/.local/share/applications/`. The Git alias is local
to the selected repo. Re-run the installer to update or change the target; the
single desktop icon then targets the latest selection. Each repo's Git alias
keeps its own selected target.

## Shared implementation and updates

The full shell installer automatically installs the icon when missing and
refreshes it on every `synczshrc`, including when the `.zshrc` block has not
changed. It preserves an existing standalone selection. Both versions call
`gobreak.py`; edit that source in the repo, then run `synczshrc` to refresh both
installed copies. Updating with the standalone installer also refreshes an
existing enrolled shell runtime. Installed copies are generated files; edit
the repository source rather than those copies. Updates do not Git-push or lock.

For standalone-only updates, pull the helpers clone and re-run the install
command with the same `--repo`; saved includes and excludes persist:

```sh
git -C 42-gobreak pull --ff-only
python3 42-gobreak/install_gobreak.py --repo "$HOME/path/to/your/repo"
```

The full shell's `gobreak` keeps its multiple-repository behavior and private
chat backup. Its configuration belongs in `~/.42-shell-settings.zsh`:

```sh
export GOBREAK_REPOS_ROOT="$HOME/Documents"
export GOBREAK_INCLUDE_REPOS="$HOME/extra-repo:$HOME/another-repo"
export GOBREAK_EXCLUDE_REPOS="$HOME/Documents/skip-this:$HOME/skip-too"
```

Include/exclude variables are colon-separated absolute paths. They add to saved
installer selections and the existing exclusions. `GOBREAK_REPOS_ROOT` chooses
the scan root (an empty value disables scanning). The main shell defaults it to
`DOCUMENTS_REPOS_ROOT`, normally `~/Documents`; standalone defaults to no scan.
Explicitly exported variables also affect standalone launches in that desktop
session. Exclusions govern repository pushes; the full mode's private chat
backup remains a separate step. Reload `.zshrc` after changing shell settings.
`python3 install_gobreak.py --shell` explicitly switches the icon to full-shell
mode; ordinary shell sync preserves its current mode.

## Uninstall or remove

Deleting the downloaded `~/42-gobreak` folder alone **does not uninstall** the
icon, installed helpers, or Git alias. To remove the standalone installation:

1. Right-click **Go Break (Git push + lock)** on the dock and choose
   **Remove from Favorites**.
2. In each repo where you installed Go Break, run this to remove its Git alias
   (a subdirectory of that repo also works):

```sh
git config --local --unset-all alias.gobreak
```

If the alias is already absent, that command makes no change. Repeat it for
each configured repo before deleting the helpers, so aliases do not point at
a missing launcher.

3. Copy and paste this removal chunk to delete the installed helpers, saved
   selections and application icon. It respects `XDG_DATA_HOME` if you use it:

```sh
if [ -d "${XDG_DATA_HOME:-$HOME/.local/share}/42-gobreak" ]; then
  rm -r -- "${XDG_DATA_HOME:-$HOME/.local/share}/42-gobreak"
fi
rm -f -- "${XDG_DATA_HOME:-$HOME/.local/share}/applications/local.42-gobreak.desktop"
```

4. Optionally delete the downloaded helpers clone too, if you no longer want it:

```sh
rm -r -- "$HOME/42-gobreak"
```

Check for any edits you want to keep in that clone before deleting it.
Removal does not undo existing commits or pushes, or remove your project repos.

**Using the full shell setup too?** This removes only the standalone installation.
The main shell's `gobreak` function remains, and the next `synczshrc` reinstalls
the standalone helpers and icon automatically. Its saved selections and opt-outs
were deleted above, so that reinstall uses the full-shell scope and default
pinning/Codex preferences. See the [full shell guide](../README.md) if you want
to remove the main shell setup as well.

## Locking on errors and interruption

Both `gobreak` and `leavesch` always attempt `ft_lock` on success, save/push
errors, cancellation, and catchable INT/TERM/HUP signals. Shell/desktop wrappers
also cover a missing Python executable, missing runtime, or interpreter startup
failure. Standalone Python invocations use a `finally` guard, including argument
and configuration errors. Each entry point owns one lock attempt, avoiding a
second lock after you unlock. Desktop errors are shown after the lock attempt.

A failed EOD save keeps tmux open. Successful `leavesch` still closes tmux;
`gobreak` keeps it running. SIGKILL, power loss, or a missing/broken `ft_lock`
cannot be made into a guaranteed successful lock by a script.
