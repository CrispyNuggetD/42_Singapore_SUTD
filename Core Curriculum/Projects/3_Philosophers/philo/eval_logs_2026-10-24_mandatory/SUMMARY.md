# Mandatory evaluation review — 24 October 2026

**Historical review:** its blockers were subsequently resolved. See the
[final validation report](../eval_logs_2026-10-24_final/SUMMARY.md).

**Verdict: not ready to label mandatory complete. No commit or push performed.**

The named evaluation-sheet runtime cases passed, but the submission tree has a
Norm failure and two additional valid timing cases exposed a deterministic
starvation problem. Production source files and Makefiles were unchanged during
this review; hashes are recorded in `source_hashes.json`.

## Basis and scope

- Evaluation sheet supplied by the user: https://github.com/Dsite42/philosophers/blob/master/philosophers%20eval%20sheet.pdf
- Local subject version 13.0, including its allowed functions and README requirements.
- All valid runtime cases used at most 200 philosophers and durations >= 60 ms.
- Native runs were sequential, without artificial thread pauses or injected load.
  `stdbuf -oL` wrote line-buffered output to logs, which differs from a terminal.
- Separate Valgrind runs used generous deadlines for quota cases; instrumentation
  results are memory/thread checks, not native performance measurements.
- Startup-failure injection used a separate temporary executable. It did not
  modify the production executable or introduce failures into native timing tests.

## Results

| Area | Result |
| --- | --- |
| Clean rebuild, root Makefile forwarding | Passed |
| Repeated make | No unnecessary compilation or relinking |
| Norm: all compiled C files and header | Passed |
| Norm: complete submission source directory | **Failed: tracked study example** |
| Named sheet cases and two-philosopher timing variations | 25/25 passed |
| Argument handling samples | 15/15 passed |
| Additional valid timing/count cases | 6 passed, **2 failed** |
| Memcheck: quota, single death, multi death, invalid input | 4/4 clean; no leaks |
| Helgrind: quota and death | 2/2 reported zero errors |
| DRD: quota | Reported zero errors |
| Injected startup failures under Memcheck | 8/8 exited with error and no leaks |

### Sheet runtime details

- `1 800 200 200`: one fork message, no eating, one death, clean exit.
- `5 800 200 200`: survived three separate 10-second observations.
- `5 800 200 200 7`: quota completion in five out of five runs.
- `4 410 200 200`: survived three separate 10-second observations.
- `4 310 200 100`: expected death and clean exit in three out of three runs.
- Two philosophers: three repetitions each of `310 200 100`, `250 150 150`,
  and `60 120 60`, plus quota completion with `800 200 200 7`.
- The tested death timestamps matched the logged last-meal/start deadlines
  (0 ms logged lateness). These are log-based checks, not independent proof of
  the physical display latency or of the 10 ms requirement on every schedule.

Logs had valid state-message formatting, in-range IDs, nondecreasing timestamps,
and no state messages after the unique death message. Every observed eating
entry followed two fork messages. Actual fork identity/ownership is supported
by source inspection and the thread tools, not recoverable from these messages.

For quota tests, the checker requires normal exit, no death/error, and at least
K eating entries per philosopher. Source inspection confirms that the counter
used by the monitor increments only after completing the eating wait. Logged
meal starts alone are not a separate measurement of completed meals.

Survival observations were deliberately terminated at their time limit; those
runs do not establish leak-free shutdown. Memcheck used naturally terminating
quota, death, input-error, and startup-failure runs instead.

## Blocker 1: Norm in a tracked study file

`philo/src/increment with lock study example.c` is not compiled by the Makefile,
but it is tracked inside the mandatory source directory. Norm reports:

- Missing/invalid 42 header.
- Consecutive newlines.
- Missing newline after the final brace.

Move this obsolete exercise out of the submission C sources, or preserve it as
text study notes. Its unused status does not make the full directory pass Norm.
See `norm_submission.txt`.

## Blocker 2: the extra wait double-counts time already spent sleeping

Both of these valid cases died instead of reaching quota:

```sh
./philo/philo 5 800 200 400 3
./philo/philo 3 800 200 400 3
```

P1's observed sequence in each:

| Time | Event |
| --- | --- |
| 0 ms | Starts eating |
| 200 ms | Starts sleeping |
| 600 ms | Announces thinking |
| 600–800 ms | The routine adds a full 200 ms extra wait |
| 800 ms | P1 dies |

The sum is `200 eating + 400 sleeping + 200 extra waiting = 800 ms`.
This consumes the entire death budget before another meal can start, even with
ideal scheduling. For comparison, these timings admit a three-slot repeating
schedule with meal-start gaps of 600 ms: `[P1,P3] -> [P2,P4] -> [P5]` for five,
or `[P1] -> [P2] -> [P3]` for three. Each philosopher then has the required
400 ms between the end of one meal and the start of the next.

The current extra-wait calculation was derived for equal eating and sleeping
durations. It must account for time already spent sleeping, and the intended
schedule must be reconsidered where a two-slot return cannot accommodate that
sleep. Simply reducing every wait by an arbitrary percentage would not explain
or fix the general timing relationship.

The traces are `five_long_sleep.log` and `three_long_sleep.log`.

## Other evidence

- Counts 3, 7 and 9 completed quota 3 with `800 200 200`.
- Counts 199 and 200 completed quota 3 with `2000 60 60`.
- `5 800 200 100 7` completed quota in this run.
- The linked executable's application function imports match the subject's
  allowed list. C runtime startup symbols are listed separately by the linker.
- Source review found one worker per philosopher, one mutex per fork, fixed
  acquisition order, protected meal/death state, serialized output, and no
  global shared simulation-resource variables.
- Fault injection covered both allocation sites, shared and partial fork/meal
  mutex initialization failures, and failure on the first/third thread creation.
  The wrapper is saved as `startup_failure_wrapper.c.txt` for reproducibility.
- README has the required first line, Description, Instructions, Resources,
  and AI-use disclosure. This is not a substitute for the oral defense.

## Limits and next steps

Sampled clean Helgrind/DRD runs are evidence, not proof against every possible
interleaving. Long-term survival, scheduler overload, terminal/output stalls,
and every library-failure combination were not exhaustively tested.

Resolve the study-file Norm issue and unequal-duration extra-wait behavior,
then rerun affected runtime checks and the submission Norm check. The requested
commit message `mandatory part complete` is withheld because the conditional
"if all is OK" is not satisfied.
