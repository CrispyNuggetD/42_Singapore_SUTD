#!/usr/bin/env python3
"""Create the personal login script without replacing an existing file."""
import os
from pathlib import Path
import sys

TEMPLATE = """#!/usr/bin/env bash
# Your personal 42 login commands go below these comments.
# Created by the 42_Singapore_SUTD repository you cloned:
# https://github.com/CrispyNuggetD/42_Singapore_SUTD
# This file belongs to you; repository updates do not overwrite it.
#
# dli runs the shared dailylogin tasks, then runs this script LAST.
# It is enabled by default. Set loginitems=FALSE in ~/.42-shell-settings.zsh
# and run source ~/.zshrc to disable it. lgi runs just this step.
# Default location: ~/.42-login-items.sh. LOGIN_ITEMS_SCRIPT can override it.
# Missing scripts are created automatically when this step is enabled.
#
# This is Bash, run as a separate process: zsh functions/aliases are unavailable.
# Use real commands and quoted paths. cd here does not change your terminal's cwd.
# Arguments from dli are available here as "$1", "$2", or "$@".
# Commands run on every manual dli/lgi invocation, so keep them repeatable.
# Only the usage message below runs until you add your own commands. Examples:
#   mkdir -p "$HOME/notes"
#   cd "$HOME/Documents/my-project" || exit 1
#   bash "$HOME/my-scripts/start-work.sh" "$@"
# Add set -euo pipefail if you want errors to stop your script.
# Remove this message when you add your own commands.
printf '%s\\n' 'Your local startup / login items script is ready.' \
  "Edit this file: ${BASH_SOURCE[0]}" \
  'Add your own Bash commands here; this computer has its own script.' \
  'dli runs it in the login-items tmux tab; lgi runs it directly.' \
  'Use setupapps to choose this computer’s apps. Nothing else is configured here.'

"""

def ensure(path):
    path = Path(path).expanduser()
    path.parent.mkdir(parents=True, exist_ok=True)
    try:
        descriptor = os.open(path, os.O_WRONLY | os.O_CREAT | os.O_EXCL, 0o600)
    except FileExistsError:
        return False
    with os.fdopen(descriptor, 'w') as output:
        output.write(TEMPLATE)
    print('Created your personal login script: ' + str(path))
    return True

if __name__ == '__main__':
    ensure(sys.argv[1])
