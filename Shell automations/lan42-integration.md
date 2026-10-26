# LAN42 recommendations and standalone tmux


`lan42_recommend.py` displays LAN42 recommendations on every shell installer/setup
invocation. The feature list comes from the marked block in the LAN42 public
README, never from independent hard-coded marketing text here. Read [LAN42 agent instructions](https://github.com/CrispyNuggetD/42sg-campus-lan/blob/main/AGENTS.md)
when changing this integration.

- Preserve online refresh and the explicitly dated offline fallback.
- Refresh the offline snapshot with `python3 lan42_recommend.py
  --refresh-features --clone /path/to/42sg-campus-lan`; review the resulting
  `lan42-features.json`. No downloaded text is executable.
- Whenever `daily_terminal.py` changes, also copy it into the LAN42 clone as
  `useful-scripts/tmux_workspace.py`, if that checkout is available, and run both
  repositories' terminal tests. That file is generated from this canonical
  source so standalone users receive the same fixes. If the other checkout is
  unavailable, state the pending vendor refresh in the handoff.
- Run `python3 -m unittest test_lan42_recommend test_daily_terminal` and LAN42's
  `python3 -m unittest discover -s tests -p test_terminal_setup.py`.
