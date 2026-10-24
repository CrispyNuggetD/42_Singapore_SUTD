# Debug and evaluation files

These files stay in Git for reference, but are excluded from the school submission.
The mandatory program and its Makefile remain in `../philo/`.

- [Copy-and-paste test cases](test_cases.txt): evaluation cases first, then extra tests.
- [Final validation report before the file regrouping](eval_logs_2026-10-24_final/SUMMARY.md).
- [Long-sleep experiments](eval_logs_2026-10-24_long_sleep/summary.json).
- `eval_logs_*`: archived reports, logs, test runners and source snapshots.
- `old_*_death.log`: earlier starvation examples kept for comparison.

Run the commands in `test_cases.txt` from `philo/`, beside `./philo`.
New command-list logs go into `debug/eval/` and `debug/extra/`.
Rerunning a command overwrites its corresponding log.

Python runners locate the project relative to their own file. Rerunning an
archived runner tests the current executable and overwrites results in that
runner's directory. Archived snapshots and hashes describe the source tested
at the time; they are not updated merely because functions move between files.
