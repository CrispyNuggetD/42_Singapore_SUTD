# Daily Codex maintenance

`start_codex` and `start_mailbox` call the shared `update_codex` helper before
opening or checking their windows. Both official standalone packages (CLI and
app-server daemon) track `latest` through OpenAI's installer. A lock serializes
checks; only a successful check of both packages is cached for 24 hours. Failure
is visible and retryable, while the existing installation remains available.
Run `update_codex --force` for an immediate check. Personal CODEX_HOME and
CODEX_INSTALL_DIR values are preserved. No project repository pulls are added.

The mailbox CLI uses the same stable standalone command as the interactive pane.
A running CLI or daemon is not forcibly restarted. A newly selected package is
used by subsequent launches; daemon version can be checked with
`codex app-server daemon version`. IDE extension updates remain owned by VS Code.

Exited Codex/mailbox wrappers now exit too, leaving tmux's retained output visible
and marking the pane dead. The existing terminal helper recovers dead windows in
place on the next start, preserves live windows, and prevents duplicate launches.
Active mailbox claims remain owned by their live worker.

The Mac processor vendors this updater in `bridge/codex_update.py`. Keep those
copies identical. Its launchd startup calls the helper before claims, with
existing KeepAlive supervision. Install the shared Mac source locally using its
processor installer; Linux changes do not deploy to the Mac.
