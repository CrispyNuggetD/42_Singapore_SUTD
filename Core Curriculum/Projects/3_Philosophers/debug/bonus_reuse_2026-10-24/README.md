# Bonus reuse checkpoint

Later update: semaphore setup and partial-initialization cleanup are now wired
into main; see [the cleanup checkpoint](../bonus_cleanup_2026-10-24/README.md).
The notes below record the earlier reuse stage.

This is the manual reuse/scaffolding stage, not a working bonus simulation.
`make bonus` (project root) or `make -C philo_bonus` builds it. Valid input
currently reports that the simulation is not implemented and exits with status 1.
Mandatory implementation files were not changed.

## What was copied or adapted

| Bonus file | Origin and changes |
|---|---|
| `input_output_bonus.c` | Mandatory parser, positive integer conversion and partial-write helper. Changed included header; behavior retained. The writer originally came from Push_swap, not Pipex. |
| `time_bonus.c` | Mandatory `now_ms` unchanged. `wait_ms` retains the elapsed-time polling loop but omits the mandatory shared-stop lookup; it only waits/reports failure. |
| `process_bonus.c` | Pipex `init_pipeline`, `spawn_command`, and `wait_pipeline` patterns, marked `/* From pipex */`. Added all-child PID bookkeeping and waiting for any child. |
| `cleanup_bonus.c` | Pipex failure-path reaping pattern, marked `/* From pipex */`. New bonus behavior kills unfinished children before reaping them. |
| `error_bonus.c` | Mandatory enum/message lookup and stderr writer, with bonus usage and process errors. No Pipex `perror`. |
| `main_bonus.c` | Validates input and explicitly reports the unimplemented simulation. It does not fork pretend philosophers. |
| `philo_bonus.h` | Copied config type; focused status enum, child exit codes, PID resource struct and prototypes. No mandatory mutex arrays or shared runtime struct. |
| `philo_bonus/Makefile` | Adapted mandatory Makefile: independent binary, object directory, dependency files and explicit source list. |
| Root Makefile | Added `bonus`; `clean` and `fclean` cover both implementations. Default build stays mandatory. |

Pipex's field-by-field initialization was adapted to allowed `memset`; no libft
or `ft_memset` dependency was added. `perror`, `wait`, `pipe`, `dup2`, `execve`
and file-descriptor cleanup were not copied: they are unnecessary here and those
functions are not in this bonus's allowed list. `waitpid` replaces `wait`.
`WIFEXITED`, `WEXITSTATUS`, `WIFSIGNALED`, `WTERMSIG`, and `errno` are macros;
`errno == EINTR` permits retrying an interrupted wait. Pipex did not have that retry.

## Process helper contracts

- `init_children(&children, config.count)`: after input validation; allocate a
  zeroed PID array. Call once on an empty resource object, not on live children.
- `fork_philo(&children, index, &pid)`: parent calls once per unused zero-based
  index. Both processes receive STATUS_OK on successful fork. The child has
  `pid == 0`: run its future philosopher routine and call `exit(...)` rather than
  continuing the parent's spawn loop. Only the parent records the child's PID.
- `wait_next_child(&children, &exit_code)`: parent calls while `alive > 0`.
  Waits for any child, clears its PID slot, decrements alive and decodes its exit.
  This parent must not own unrelated children, since waitpid(-1) can reap any child.
- `cleanup_children(&children)`: parent-only, terminate every remaining tracked
  child, reap them and free the PID array. Also works after all children finished.
  Retains bookkeeping on cleanup failure rather than claiming everything is gone.
  A child must not call it: its inherited PID array describes siblings.

Return values and child outcomes are deliberately separate:

| Value | Meaning |
|---|---|
| wait_next_child returns STATUS_OK | Successfully collected a child; inspect exit_code next |
| CHILD_DONE (0) | Future routine completed its meal quota |
| CHILD_DIED (1) | Future child death monitor announced starvation |
| CHILD_ERROR (2) | Future child encountered an operational error |
| 128 + signal number | Child terminated by a signal |

These child outcomes are a proposed wiring contract, not currently emitted by
an implemented philosopher routine. The tests supply small substitute children.

## What still changes from mandatory

| Mandatory behavior | Bonus behavior to implement |
|---|---|
| init fork and meal mutex arrays | Open named semaphores; forks starts at N, pair-acquisition gate and output gate at 1 |
| pthread_create one philosopher | fork_philo, then child routine + exit |
| Specific first/second fork pointers | Two sem_wait(forks) calls under an acquisition gate; two sem_post(forks) on normal release |
| Main scans every philosopher's meal fields | Each child monitors its own meal state, e.g. via one monitoring thread; use a semaphore to protect local thread-shared meal fields |
| Shared all_stop and first_error variables | Ordinary variables are private after fork; child exits communicate outcomes to the parent |
| Main joins philosopher threads | Parent waits for any child; death/error stops all remaining children; quota success waits for everyone |
| printf under printing_lock | Printing semaphore and a deliberate death/output ordering protocol; copying the old lock names is insufficient |
| pthread_mutex_destroy and free | Reap processes, sem_close handles and sem_unlink owned names; track partial initialization |
| Odd/even fork order and rotating odd-count waits | Not copied: forks have no identities in bonus; choose acquisition/deadlock prevention for the shared pool |

The one-philosopher case still cannot acquire two tokens. Starvation during eating
still needs detection. The pair gate must be released after acquisition, not held
through eating, so eating can remain concurrent. Killing a process does not return
its semaphore tokens: on death/error we stop the entire simulation, not continue
with the remaining philosophers. Fairness and output ordering still need attention.

The next implementation step is semaphore setup/cleanup, followed by the child
routine and monitor, and finally the parent loop wiring. A future child routine
also closes its inherited semaphore handles and frees its own allocations on its
normal exit path; abrupt termination is handled by the parent's overall cleanup.

## Checks performed

- Build with -Wall -Wextra -Werror; second make did not relink.
- Norm passed every bonus .c and header.
- Parser rejected missing, empty, negative and overflowing arguments.
- Valid input explicitly reported the unfinished scaffold.
- Process harness: all children complete, nonlast child reports death, partial
  fork failure, and signalled child status decoding; no children left to reap.
- Valgrind Memcheck passed normal and partial-fork-failure harness cases.

The harness is `process_harness.c.txt` in debug, not a submission source. Its second
fork failure is intentionally injected with --wrap=fork. Harness runs do not test
semaphores, philosopher scheduling, death timing, or completed bonus correctness.
