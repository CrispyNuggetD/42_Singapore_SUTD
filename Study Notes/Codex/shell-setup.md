# Personal shell setup

Install from your clone, wherever you put it:

```sh
python3 "/path/to/42_Singapore_SUTD/sync_zshrc.py"
source ~/.zshrc
setupzshrc
```

The installer enables mouse scrolling in `~/.tmux.conf`, preserving existing
configuration and backing it up before changes. For an already running tmux
session, run `tmux set -g mouse on` once. Scroll up to view pane history and
press `q` to leave scroll mode. Hold Shift while dragging to use your terminal's
normal text selection.

`setupzshrc` shows the author's example destinations and asks for your own email,
repository paths, project folder, and tmux session name. It also asks which tasks
you want: Codex, mailbox, LAN42, Stayon, repository pulls, main-repository
commit/push, project cleaning/VS Code, Chrome, login reports, and brightness.
Brightness has its own percentage setting. Optional services and automatic
updates default to **off**; login reports and progress reporting default to on.

Review the summary and save. Setup only saves configuration; it does not launch
services, install Codex, pull repositories, or change brightness. Run `dailylogin`
when ready. Calling `dailylogin` before setup opens setup in an interactive
terminal, then returns so you can review before running tasks. Noninteractive
calls stop with setup instructions.

## Personal files

- `~/.42-shell-settings.zsh`: settings and completion flag. Run `setupzshrc` again
  to change them, or edit the file and reload `.zshrc`. Each save backs up the
  previous settings file. No username check is used.
- `~/.local/share/42-shell/`: installed copies of the setup wizard, tmux helper,
  updater, and shared shell template. The installer copies these files explicitly;
  they are not links into the clone.
- `~/.zshrc`: loads personal settings and defines the shell commands. Shared
  updates preserve content outside the marked block and do not replace the
  personal settings file.

`ZSHRC_SETTINGS_FILE`, `ZSHRC_RUNTIME_ROOT`, and `ZDOTDIR` support alternative
settings, helper, and `.zshrc` locations. Install to a different runtime directory
with `sync_zshrc.py --runtime /your/path`.

## Updates and deleting the clone

`ZSHRC_SHARED_ROOT` is the update clone. `MAIN_REPO_ROOT` is your own project
repository; they can be different locations. The setup wizard explicitly asks
whether `autoupdatezshrc` should be on. When on, `dailylogin` pulls the update clone
and refreshes the installed helpers and marked `.zshrc` block. When off, no
automatic helper update happens. `synczshrc` remains available for manual updates.

Deleting the update clone leaves the installed helpers available, including
`setupzshrc`, `workterm`, and `workwindows`. A missing update clone is reported and
skipped, even if automatic updates were on. Clone it again at the configured path,
or run setup to choose a new location, to resume updates. A pull failure from an
existing clone still stops the workflow.

Project commands still require their project files. Mailbox, LAN42, and Stayon
still need their separately installed applications. Deleting those applications
or a project repository removes that particular task's dependencies; the installer
does not copy private projects or whole service repositories.

## Codex workspace

The private repository holds general Codex work. New setups and shared defaults use `~/Documents/42_Singapore_SUTD_hnah_private/42` for both
`dailylogin`'s Codex window and the interactive `codex` command. Prepare it with:

```sh
git clone git@github.com:CrispyNuggetD/42_Singapore_SUTD_hnah_private.git ~/Documents/42_Singapore_SUTD_hnah_private
mkdir -p ~/Documents/42_Singapore_SUTD_hnah_private/42
```

`CODEX_START_DIR` in personal settings overrides this default. Existing settings
are preserved during setup and sync; update that value explicitly to migrate an
older installation. Use `codex --cd /path/to/project` (or `-C`) for an individual
launch in another directory. General reports and exports belong in the Codex
workspace; school project commands continue to use `MAIN_REPO_ROOT`.

When `openproject` is enabled, `dailylogin` runs `curproj` as its final task,
leaving the invoking shell in `MAIN_REPO_ROOT/PROJECTS_ROOT/current_proj` after
repository syncing. This is independent of the Codex workspace.

`adjust_brightness` is the command; `adjustbrightness` is its enable/disable
setting. The setting is excluded from command-name completion.

## Leaving school

Run `leaveschool` to copy new or updated Codex session JSONL files into the
separate private repository's `42/codex_cache/sessions`, commit all non-ignored
changes in repositories below `DOCUMENTS_REPOS_ROOT` (default `~/Documents`),
and push each current branch to its configured upstream. Nested repositories
are saved before their parents. The Mac mailbox and symlinked subdirectories
are excluded. Repositories without an upstream, detached heads, unfinished Git
operations, and rejected pushes are reported; other repositories still run.
No force pushes or automatic merges are performed.

After successful backup and pushes, **all sessions on the selected tmux server
are closed, including attached sessions**. Any save failure keeps tmux open so
you can fix the error and retry. `DAILY_TMUX_SOCKET` selects an explicit server;
otherwise the current/default tmux server is used. `quithidden` remains available
separately for closing detached sessions only.

`CODEX_PRIVATE_REPO_ROOT` may select another location of the same private clone.
The helper rejects a destination in the shared repo or behind a symlink. Only
`CODEX_HOME/sessions/**/*.jsonl` is copied (default `~/.codex/sessions`); credentials,
databases, attachments, and other Codex files are excluded. Existing backups
are retained. An active chat can continue writing after the snapshot; its later
updates are copied on the next run. This command does not shut down the computer.


## Returning from home

On the Mac, `leavehome` exports only changed chats listed in the local school-import manifest. `leavehome --check` previews without exporting or pushing. Mac-only conversations, credentials, config and databases are excluded. The snapshots and their index live only in the private repository's `42/codex_cache/return_to_school/` directory. The command commits only that directory, preserves other staged/unstaged work, and refuses to push unrelated unpublished commits. It leaves applications open.

On Ubuntu, the updated `dailylogin` runs `importhome` before opening the project. `importhome` first pulls the private repo with fast-forward-only Git, then registers returned snapshots in the current `CODEX_HOME` using Codex's local metadata reader and `migrate-rollouts`. Original school chats remain intact; choose the returned copy to continue. Linux project paths replace Mac working-directory metadata. Restart/reload the VS Code window if the history list is stale.

For a first manual update at school:

```zsh
pullzshrc
synczshrc
source ~/.zshrc
importhome
```

With `autoupdatezshrc` enabled, normal `dailylogin` performs the update and import. The return importer needs the current private `Scripts and Testers/codex_school_import.py` helper, a Codex CLI supporting `migrate-rollouts`, and the same `CODEX_HOME` used by the VS Code extension. Set `CODEX_BIN` to the extension's Codex executable if the standalone CLI is older. No direct SQLite writes, extension setting changes, or model turns are used for importing.

Each changed Mac revision becomes a distinct school snapshot, so independent edits do not overwrite one another. Repeating a handoff is a no-op. An unchanged returned copy backed up by `leaveschool` is not imported into the Mac again. If you continue that copy at school, its new revision can be picked up on the Mac as usual. Manually created Mac chats/forks outside the import manifest are not exported.

The VS Code extension is powered by [Codex app-server](https://learn.chatgpt.com/docs/app-server). This script is a local transcript-transfer workflow using the installed CLI's migration support, rather than an automatic account-wide chat sync feature. Keep the school CLI and extension compatible; importing into another CODEX_HOME will not update the extension's history.
