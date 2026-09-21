## Current reapplication onto 42 edits (2026-09-21)

- Reapplied the discussed candidate retries while retaining upstream names:
  candidate_command, DIRECT_PATH_SUPPLIED, attempt_possible_candidates,
  copy_direct_path, and exec_failure_error.
- Connected the candidate helper, declarations, enum results, and Makefile.
  Retain the first execve EACCES, try later candidates silently, and report
  the retained path once if none succeeds. Other execution errors stop search.
- Omitted the array-free-only errno save/restore for the stated 42 target
  (glibc 2.35). Other execution-error capture remains intact.
- README now follows the current names. Older entries below are historical.
- Local mandatory and bonus builds each passed ten focused execution checks,
  including retained first-denied diagnostics and missing-interpreter status.
  Changed C files/header passed Norm and git diff --check passed.

---

## Reapplied after upstream renames (2026-09-21)

- Pulled upstream through 24f497d and reapplied the PATH retry study changes.
- Preserved upstream names: command_str is the full input command,
  exec_fail_path is the selected/retained executable path, and path in
  resolve_path is the PATH environment value.
- Execution errors now use perror(path), identifying the retained failed
  candidate. Command-not-found diagnostics still use the command name.
- Revalidation: mandatory and bonus builds each passed nine focused checks,
  including two denied candidates reporting only the first path. Changed C
  files and header passed Norm; git diff --check passed.

---

## Study naming clarification (2026-09-21)

- Replaced the overloaded PATH_FOUND result with explicit outcomes:
  PATH_READY means an explicit path is ready for the caller to execute;
  EXEC_FAILED means stop and report the retained execution error.
- The candidate helper returns SEARCH_CONTINUE when another directory should
  be tried. COMMAND_NOT_FOUND is reserved for the overall lookup result.
- Successful execve never returns any result. The retry policy is unchanged.

---

## Latest study step — retry later PATH candidates (2026-09-20)

- Implemented behavior (1) of the six-item study breakdown: attempt execution
  during PATH search and continue after EACCES, so an earlier non-executable
  file or directory does not hide a later usable executable.
- `search_directories()` builds one candidate at a time; `attempt_one_candidate()`
  attempts execution. Successful execve replaces the child and never returns.
- The first denied candidate is retained for the final diagnostic if no later
  candidate works. Other execve errors stop the search, preserving the prior
  failure classification. This is a limited retry policy, not the full error
  policy planned in behaviors (2)/(3).
- `resolve_path()` now takes the full argument array. For PATH searches,
  EXEC_FAILED means a failed execution's path and errno are available for final
  reporting; explicit slash-containing paths are still executed by the caller.
- Existing F_OK prechecks remain: inaccessible candidates that fail that check
  are still skipped. Empty entries and empty/missing PATH are unchanged.
- Mandatory and bonus builds each passed seven focused checks: denied then
  usable, directory then usable, first usable wins, denied then missing,
  explicit denied path, all missing, and normal explicit execution.
- Changed C files and header passed Norm. No full leak/FD audit performed.

---

## Latest resume point — execution-error classification

- Task (2) is implemented for the agreed Pipex scope: preserve the execve
  error, distinguish missing paths from other execution failures, report the
  original error, free allocations, and exit with the selected status.
- `exec_failure_status()` chooses the status; `exec_failure()` handles reporting,
  cleanup, and exit. Passing errno by value preserves it before helper calls.
- Both builds and five failure cases passed against local Bash 5.1.16; normal
  execution, closed-stderr status preservation, and execute.c Norm also passed.
- Next: discuss task (3), PATH candidate selection and empty/missing PATH
  behavior, before implementation. Do not expand into Minishell behavior.
- Limits: no custom Bash diagnostics or ENOEXEC script fallback; the access()
  check after execve() can race with filesystem changes. Full audits remain.

---

# Pipex checkpoint — 2026-09-18

This is a development checkpoint, not a claim of final subject compliance.
The user is reviewing and learning the implementation; explain proposed behavior
changes and preserve their saved edits. Do not treat this checklist as automatic
authorization to implement every remaining item.

## Completed in this checkpoint

- Added `ryker_ft_get_next_line(t_gnl_info *, char **)` with explicit `GNL_LINE`,
  `GNL_EOF`, and `GNL_ERROR` results, caller-owned state, and explicit cleanup.
  Reuses original GNL helpers; the original subject-compatible API remains.
- Integrated it into heredoc and removed the local byte-at-a-time GNL implementation.
  `heredoc_cleanup()` releases the line and stash and returns the caller's status.
  Write errors no longer overwrite a GNL-specific result.
- Consolidated heredoc names and write-FD names, and retained the full-line printf
  helper as `write_line_guaranteed()` (success means fully written; errors can
  occur after partial output).
- Added `ryker_ft_create_open_unique_file()` in `ryker_ft/file_unique/`:
  exclusive creation, numeric collision retries, no signed-counter overflow,
  NULL output on failure, preserved errno, and no unlink of unowned candidates.
- Integrated file creation into Pipex; report reopen errors before cleanup.
- Reorganized library documentation into component indexes and dated updates.
  Synced reusable components into main libft, Pipex, FdF, and push_swap copies.
- Completed shell task (1): `resolve_path()` returns `PATH_FOUND`, `COMMAND_NOT_FOUND`,
  or `ALLOCATION_FAILED`, with allocated output only on success. Allocation failures set
  `ENOMEM`, report the system error, release arguments, and exit 1 rather than 127.

## Shell-correctness task status

### (2) Classify execve failures — scoped implementation complete

The failed execve error is captured by value in `exec_failure()` before any
reporting or cleanup. `exec_failure_status()` defaults to 126; for ENOENT it
checks the path with access(F_OK), choosing 127 only if that check also fails
with ENOENT. The original errno is restored for perror, then allocations are
freed and the selected status is returned through exit.

Mandatory and bonus tests matched local Bash 5.1.16 for missing explicit paths,
non-executable files, directories, missing script interpreters, and a
non-directory path component. Diagnostics retain the original system error;
custom Bash wording and ENOEXEC script fallback are not implemented. The
existence check can race with filesystem changes. A closed-stderr check retained
status 127, normal execution passed, and execute.c passed Norm.

### (3) Improve PATH search

The current search stops at the first candidate for which `access(F_OK)` succeeds.
A non-executable candidate can hide a later usable command. Consider executing
candidates in order and retaining relevant permission failures. Preserve empty
PATH entries (current-directory semantics); `ft_split(..., ':')` currently drops
them. Define missing/empty PATH behavior with reference-shell tests. Keep lookup
allocation errors distinct from not-found while changing the algorithm.

## Other remaining work

- Norm: `main_bonus.c` has whitespace on an empty line and an overlong line;
  `usage_printer.c` has an overlong line. These three issues remain at checkpoint.
- Refresh older README architecture/build/permission prose: shared pipeline code
  and current output permissions should agree with the final implementation.
- Complete leak, FD ownership, and failure-path audits, including interrupted waits,
  fork/pipe errors, output failures, and full cleanup across parent/child processes.
- Review command parsing against the subject: splitting only on spaces does not
  implement shell quoting. Do not claim complete shell equivalence.
- Temp-file unlink failures are still ignored. Predictable names and the
  close/reopen-by-path sequence have limitations documented in file_unique.
- Buffered GNL can consume beyond a limiter; cleanup discards those bytes.
  Reuse for Minishell command input needs a deliberate buffering policy.
- Original GNL's allocation-failure cleanup is separate from the extension's
  cleanup; this checkpoint does not claim to fix all original-API failures.

## Directory wrapper: WIP, excluded from Pipex

`ryker_ft_create_open_unique_file_in_dir.c` is experimental study material.
The user has not reviewed/understood it sufficiently to take ownership of it;
passing tests does not make it approved or submission-ready.

- Retained under main libft's `ryker_ft/file_unique/`, with WIP comments.
- Also flagged WIP in the other retained library copies; excluded by `WIP_SRCS`
  from normal builds and absent from public headers.
- Entirely absent from Pipex's embedded source tree. Do not re-copy it into Pipex.
- `mkdir()` and `stat()` are not authorized Pipex runtime dependencies.
- Do not promote or activate this wrapper without the user's review.

## Validation and limits

During this session: mandatory/bonus builds, normal pipelines, heredoc append,
empty/large input and limiter-at-EOF checks passed. GNL extension passed focused
ASan/UBSan and injected allocation-failure checks at buffer sizes 1, 67, and 4096.
The unique-file helper passed collision, preservation, invalid-path, mode, and
concurrent-creation tests. Lookup fault injection covered split/join/duplicate
allocation failures and verified exit 1 with a lookup diagnostic, not exit 127.

ASan required Homebrew LLVM 23.1.1 with an explicit installed SDK; the Apple
Clang 16 sanitizer runtime failed even on a minimal program. Details are in the
GNL README. Temporary test harnesses are not committed test infrastructure.
All library builds passed; WIP archive members and Pipex mkdir/stat imports
were checked absent. Full Pipex Norm still has the three issues listed above.
These results are focused validation, not a complete shell-conformance or
memory-failure audit.
