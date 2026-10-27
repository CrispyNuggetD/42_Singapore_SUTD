# Dailylogin on a fresh computer

Install the shared shell block using its installation instructions, then run
`dailylogin` in a terminal. Sourcing `.zshrc` never installs packages.

Before setup or updates, `dailylogin` and `dli` check Python 3, tmux and Git.
Enabled Codex startup also checks curl; enabled project startup checks make.
Missing tools are installed automatically with `apt-get update` and
`apt-get install -y` on Ubuntu, or `brew install` on macOS. Ubuntu uses sudo
when needed; an interactive terminal can ask for your password. Unattended
runs use sudo -n and report the recovery command if privileges are unavailable.
Install failures stop startup before any daily tasks run. Successful installs
are checked again before continuing. Existing tools are left alone.

On a Mac without Homebrew, the message links to Homebrew setup; rerun dailylogin
after installing it. Other package managers receive a list of missing tools.

First-run setup asks for your own locations and optional services. You do not
need the author's private repositories. Leave mailbox and Stayon disabled
unless you have installed and configured them. LAN42, Chrome, project and
service failures explain how to configure or disable the corresponding task.
The original error remains visible; a failed task stays failed in the summary.
Personal enrollment, repository access and project-specific build failures
need the user's configuration rather than guessed credentials or paths.

## Campus hours on Mac and other accounts

Enable `loginreports` with `setupzshrc`. `hourslogged` and `loginbefore12`
use the configured `FORTYTWO_LOGIN` (then the email login or local username),
with an existing local API token or an authorized LAN42 API configuration.
They work independently of the OS username being hnah or the OS being Linux.
API access and campus authorization still apply. A fresh API snapshot may be
shared for five minutes; pass `--refresh` to fetch again.

If API access fails or is unavailable, the report prints a recovery message and
skips. For the Linux `hnah` OS account reporting `hnah`, API failure retains the
explicitly labelled local-last fallback. On Macs and other accounts, local
records are comparison data only and never replace API hours. Dailylogin counts this as skipped and continues other
tasks. People without a 42 account can disable `loginreports`.

On a Mac whose OS username differs from the campus login, set
`export FORTYTWO_LOGIN=YOUR_42_LOGIN` in your personal
`.42-shell-settings.zsh`. This changes only report identity, never the OS account.
LAN42-backed history requires an approved host and local operator enrollment,
and is restricted to that operator's own verified Singapore account.
