# Dailylogin on a fresh computer

Install the shared shell block using its installation instructions, then run
`dailylogin` in a terminal. Sourcing `.zshrc` never installs packages.

Before setup or updates, `dailylogin` and `dli` check Python 3, tmux and Git.
Enabled Codex startup also checks curl; enabled project startup checks make.
Ubuntu does not run sudo or install/update system packages. Missing tmux is
offered with a `[y/N]` prompt, then prepared under `~/.local/share/42-tools/tmux` using `apt-get download` and
`dpkg-deb --extract` with existing repository metadata. The private wrapper is
checked before use and its library settings apply only to tmux. Network/mirror
restrictions, stale metadata or unavailable package tools can prevent recovery;
use LAN42's direct `lan42_here` launcher while resolving workspace setup.
Python, Git, curl and make are checked but not automatically installed on Linux:
load the school's available executables on PATH. Python itself must already be
available to install the shared helpers. No unsupported rootless Python install
or privileged workaround is attempted. macOS retains Homebrew recovery.

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
The hours report shows a Monday–Sunday attendance period (Day 1–Day 7)
alongside rolling seven calendar days (Day -6–Day 0). Set
`LOGIN_HOURS_TIMEZONE=Asia/Singapore` and, for a portal policy of 20 hours
per week, `LOGIN_HOURS_WEEKLY_TARGET=20` in personal shell settings. The
target is local configuration; the Intra locations API does not supply the
assigned Singapore attendance policy. The portal may count hours differently.
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
