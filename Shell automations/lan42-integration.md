# LAN42 recommendations and standalone tmux


`lan42_recommend.py` displays LAN42 recommendations on every shell installer/setup
invocation. The feature list comes from the marked block in the LAN42 public
README, never from independent hard-coded marketing text here. Read [LAN42 agent instructions](https://github.com/CrispyNuggetD/42sg-campus-lan/blob/main/AGENTS.md)
when changing this integration.

- Preserve online refresh and the explicitly dated offline fallback.
- Refresh the offline snapshot with `python3 tools/lan42_recommend.py
  --refresh-features --clone /path/to/42sg-campus-lan`; review the resulting
  `lan42-features.json`. No downloaded text is executable.
- Whenever `tools/daily_terminal.py` changes, also copy it into the LAN42 clone as
  `useful-scripts/tmux_workspace.py`, if that checkout is available, and run both
  repositories' terminal tests. That file is generated from this canonical
  source so standalone users receive the same fixes. If the other checkout is
  unavailable, state the pending vendor refresh in the handoff.
- Run `PYTHONPATH=tools python3 -m unittest discover -s tests` and LAN42's
  `python3 -m unittest discover -s tests -p test_terminal_setup.py`.

## Shared workflow and restart contract

`relogin.py` and `workspace_restart.py` are public, canonical sources installed
by `sync_zshrc.py` on Linux and Mac. Do not implement separate private copies.
A relogin ends the old tmux jobs and verified owner-managed local LAN42 node;
it must not merely detach clients and reconnect to stale processes. Do not
stop shared authentication brokers. Running work requires explicit local `y`;
`n` cancels. Tests must mock shutdown and GUI launches and isolate HOME/settings.

Dailylogin's `_dailylogin_handoff` must re-enter the newly sourced public entry
before running tasks, with `_DAILYLOGIN_REFRESHED` preventing repeated pulls.
Preserve that guard when changing the workflow. Run `test_login_update.py` and
`test_relogin.py` alongside the terminal tests. Never verify an update by running
the owner's real dailylogin or relogin; use a temporary HOME and stub every task.
