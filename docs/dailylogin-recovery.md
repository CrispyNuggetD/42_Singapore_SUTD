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
