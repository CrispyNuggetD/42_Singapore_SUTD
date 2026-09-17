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
- Completed shell task (1): `resolve_path()` returns `PATH_FOUND`, `PATH_NOT_FOUND`,
  or `PATH_ERROR`, with allocated output only on success. Allocation failures set
  `ENOMEM`, report the system error, release arguments, and exit 1 rather than 127.

## Next shell-correctness tasks — not implemented

### (2) Classify execve failures

`execute_command()` still exits 126 for every failed `execve()` call. Save errno
immediately and classify errors before cleanup. Verify against the chosen shell
with focused tests: missing explicit executable paths, non-executable files,
directories, and missing script interpreters. Do not assume every ENOENT means
the executable pathname itself is absent. Keep diagnostics tied to the real error.

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
