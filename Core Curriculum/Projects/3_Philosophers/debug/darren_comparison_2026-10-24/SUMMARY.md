# Local comparison with Darren's Philosophers

Compared 2026-10-24: Darren's local clean checkout at repository commit
`c9cb79c`, `philo/philo_threads`, against our mandatory sources at `eeb64bc`.
This is a comparison of these copies, not a judgement of another submitted version
or of other students' implementations. Neither implementation was edited.
Darren's build and failure-injection binaries were made in a temporary copy.

## Size and architecture

| Aspect | Darren | Ours |
|---|---|---|
| Mandatory C files | 7 | 10 |
| Nonblank, noncomment C source lines, excluding header | 533 | 583 |
| Default created threads | N workers + N monitors + 1 scheduler | N workers; main monitors |
| Shared state | C atomics, waiter mutex, fork reservation array, group counters | Mutex-protected meal/stop state; no central admission scheduler |
| Fork acquisition | Waiter reserves both forks before actual locking | Fixed opposite ordering for odd/even IDs |
| Timing | Sleeps for duration minus 40 ms, then busy-polls; other busy loops | Interruptible waits with 100-microsecond polling sleeps |
| Startup | Workers wait for a common start flag; timestamps set after creation | Start timestamp precedes initialization and sequential thread creation |
| Error handling | Fewer checked operations; incomplete partial-startup paths | First-error propagation and tracked partial initialization/cleanup |

Our source is about 9.4% longer by that line metric, despite having three more files.
This measures size, not correctness or cognitive complexity.

Both five-philosopher quota logs began with the same approximate 200-ms groups:

```text
[1,3] -> [2,4] -> [3,5] -> [1,4] -> [2,5] -> repeat
```

Darren's eligibility groups are {1,3}, {2,4}, {5}, but these are NOT full-meal
barriers: his scheduler advances after counting meal starts. Consequently groups
can overlap in execution, producing the rotating pairs above. Our implementation
encourages those pairs through per-philosopher initial and recurring waits.
Neither this observed trace nor a few successful tests proves scheduling fairness
for every execution. Our relative waits do not enforce a global barrier or a
bounded scheduler delay.

## Observations from tests

Native runs were sequential, with no injected thread pauses or deliberate external
load. Each survival observation lasted only three seconds and was killed at its
limit. These are spot checks, not a repeat of the complete evaluation or Valgrind
suite. Initial native runs used `stdbuf -oL`; survival repeats ran directly.

| Check | Darren | Ours |
|---|---|---|
| Build with own Makefile | Passed | Previously built and tested |
| Norminette, src and includes | init_philo exceeds 25 lines | Passed |
| 1 800 200 200 | Thinking then death; no fork message | Fork then death |
| 5 800 200 200 7 | All seven meals, no death | All at least seven meals, no death |
| 5 800 200 200 | No death in 3 seconds | No death in 3 seconds |
| 4 410 200 200 | Death at timestamp 1416 ms in initial run; three repeats survived | Initial and three repeats survived |
| 4 310 200 100 | One death at 310 ms | One death at 310 ms |
| 2 800 200 200 3 | Completed | Completed |
| Empty eating-duration argument | Accepted and ran | Rejected |
| Eating duration 4294967356 | Accepted and ran | Rejected |

Darren's `ft_atoi` performs unchecked signed-int arithmetic, so overflow is not a
valid conversion even though this build happened to continue running.

The five-philosopher quota process used about 10.11 CPU-seconds for Darren versus
0.42 for ours. CPU-seconds sum work across threads; they are not wall-clock seconds.
This is one observation, not a controlled performance benchmark. His busy loops
and default monitor count explain why fewer source files do not mean less runtime
work. Both logs had no messages after death in the native cases that died.

## Separate failure injection

These two tests intentionally returned EAGAIN from one pthread_create call via a
linker wrapper. This did NOT apply to the native tests above.

- Darren, N=2, create call 4 (first monitor): hung until the two-second test limit.
  `init_maint` calls `maint_cleanup` before main can set `startflag = -1`;
  cleanup joins the scheduler that is still spinning on `startflag == 0`.
- Ours, N=2, create call 2 (second philosopher): printed the creation error and
  exited with status 1, joining the already-started worker.

The failing roles differ because our design creates no separate monitor thread.
These tests illustrate partial-startup handling, not all possible failure paths.

## Additional source-review findings, not reproduced race failures

- Darren's print.c:16-30 releases the print mutex for ordinary messages before
  print.c:68 performs write. A worker can pass that check, another thread can
  announce death, and then the worker can write. One write per line does not make
  the stop check plus output one protected operation. Our logger holds its print
  mutex across both actions.
- Darren's handles.c:28-33 tests a last-meal snapshot before taking the waiter
  lock. philo.c:47 updates last_meal without that waiter lock. The atomic load
  avoids a torn access, but the check can still become stale before death is
  announced. Our meal-state mutex protects the deadline decision and meal update.
- Darren's init_mutexes ignores mutex initialization results. Another startup
  issue: if init_philo fails, main skips init_maint but still passes the
  uninitialized maint structure to cleanup.
- Darren's philo_sleep waits for equality of seconds before testing microseconds.
  If execution resumes after the target second has passed, this can busy-loop
  indefinitely; elapsed-time >= deadline checks avoid that problem.

These are specific weaknesses in this copy, not evidence that every short solution
is incorrect. No blanket atomics-allowed/forbidden verdict is made here.

## Assessment

Our partial-initialization tracking, single-philosopher handling, protected death
logging and input validation provide concrete value. Some repetitive checks on
correctly initialized ordinary mutex lock/unlock calls are optional defensive
complexity for this project. Our custom odd-count wait mathematics is also an
additional design choice, not a requirement for every correct solution.

Darren has a useful startup gate and explicit admission coordination, but also
more threads and shared scheduling machinery. A shorter, correct five-file
implementation remains possible. File count alone cannot decide correctness,
fairness, efficiency or whether a project was overengineered.

Raw evidence: results.json, repeat_results.json, injected_failure_results.json,
Norm logs and per-run output files in this directory. No changes were pushed.
