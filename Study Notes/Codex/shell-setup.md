# Personal shell setup

Install from your clone, wherever you put it:

```sh
python3 "/path/to/42_Singapore_SUTD/sync_zshrc.py"
source ~/.zshrc
setupzshrc
```

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
