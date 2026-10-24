# Final mandatory checks — 24 October 2026

**All checks in this final suite passed.** This report supersedes the earlier
`eval_logs_2026-10-24_mandatory/SUMMARY.md` review, whose two blockers are resolved.
The source hashes in this folder identify the exact tested implementation.

## Changes that resolved the blockers

- Preserved the unused exercise as `study_notes/increment_with_lock.c.txt`,
  outside mandatory C sources. The entire `philo/src` and `philo/includes`
  directories now pass Norm.
- `thinking_wait_ms()` subtracts mandatory sleep from the chosen rest target
  and clamps the result to zero. The routine calls it after sleeping/thinking.
- Removed dynamic fork swapping, shared meal-total state and its mutex lifecycle.
  Fixed odd/even fork order is retained on every meal.
- Corrected obsolete TODO comments about error propagation already implemented.

## Results

| Check | Result |
| --- | --- |
| `make re` | Passed |
| Repeated `make` | Nothing rebuilt or relinked |
| Full submission Norm | Passed |
| Named sheet cases / two-philosopher variations | 25/25 passed |
| Input checks | 15/15 passed |
| Additional runtime cases | 8/8 passed |
| Memcheck: normal quota, single death, multi death, bad input | 4/4 clean, no leaks |
| Helgrind: quota and death | 2/2 reported zero errors |
| DRD: quota | Reported zero errors |
| Injected startup failures under Memcheck | 8/8 clean error exits, no leaks |

The runtime and input total is **48/48**. The memory/thread/failure-injection
total is **15/15**. Detailed commands, outcomes and logs are in this directory.

### Sheet cases

- `1 800 200 200`: one fork, no meal, expected death and clean exit.
- `5 800 200 200`: three separate 10-second survival observations.
- `5 800 200 200 7`: five successful quota runs.
- `4 410 200 200`: three separate 10-second survival observations.
- `4 310 200 100`: three expected-death runs with clean exit.
- Two philosophers: three runs each with `310 200 100`, `250 150 150`,
  and `60 120 60`, plus a quota run with `800 200 200 7`.
- All tested death messages matched the logged deadline (0 ms logged lateness).

### Additional coverage

- Odd counts 3, 7 and 9 with `800 200 200 3`.
- Counts 199 and 200 with `2000 60 60 3`.
- `5 800 200 100 7`.
- The previously failing `5 800 200 400 3` and `3 800 200 400 3` both passed.
- Earlier targeted long-sleep coverage, including sleep=1000 ms, is retained in
  `../eval_logs_2026-10-24_long_sleep/summary.json` (16 successful cases).

Native tests checked message formatting, IDs, timestamp ordering, unique/final
death output, fork-message counts before eating, expected termination and meal
starts. Quota logs show at least K meal starts per philosopher; source inspection
confirms the monitor uses counters incremented after eating finishes.

The startup-failure checks used a separate temporary executable with link-time
wrappers: both malloc sites, mutex initialization calls 1/2/5/10, and thread
creation calls 1/3. Production code was not changed for fault injection.

## Scope and limits

The evaluation sheet used was supplied by the user:
https://github.com/Dsite42/philosophers/blob/master/philosophers%20eval%20sheet.pdf
The implementation was also checked against the local subject version 13.0.
Application imports match its allowed functions; no global shared simulation
resources were found. Code review confirms one worker and fork mutex per
philosopher, protected death/meal state, and serialized output.

Native runs were sequential, with no injected pauses or artificial load, using
line-buffered file output. Valgrind cases were separate and used generous quota
deadlines to avoid treating instrumentation overhead as native performance.
Timed survival observations were killed at the observation limit; leak checks
used naturally terminating runs instead.

These are finite tests, not an evaluator's grade or proof of every interleaving.
Log-based death timing does not independently measure physical display latency.
Earlier tight-margin `58/60 130 60 60` experiments sometimes died; this final
suite does not establish an unconditional 10 ms scheduler-overhead bound.
Race tools can only report what their sampled executions detect.

The project is checkpointed as `mandatory part complete` with this evidence
and these limits. Oral defense and environment-dependent timing evaluation
remain the evaluator's responsibility.
