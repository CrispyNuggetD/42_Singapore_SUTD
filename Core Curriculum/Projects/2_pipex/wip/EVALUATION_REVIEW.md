# Pipex evaluation rehearsal — 2026-09-21

Result: not submission-ready. This is a local rehearsal, not an official grade.

## Sources and scope

- User-supplied evaluation sheet, read directly from the PDF:
  https://github.com/mharriso/school21-checklists/blob/master/ng_2_pipex.pdf
  The captured sheet is dated July 17, 2021. It is the checklist used for this
  review; it does not establish the revision currently used by the campus.
- Also compared with the locally available Pipex subject v5.0. Its newer README
  requirements remain relevant even though this older sheet does not list them.
- Earlier public mirror searches are superseded by the supplied PDF.
- Tested a separate copy of working files, including uncommitted changes,
  not a clone of the official submission. No downloaded test scripts were executed.
- The sheet asks for argument/error handling, shell output and exit-status
  comparisons, required Makefile rules, Norm, and no prohibited functions.
  Bonus checks include five pipes and repeated heredoc/append runs. These are
  already covered by the recorded rehearsal; the sheet adds no prescribed
  command sequence that has not been run.
- Quote parsing and tabs are not separate checklist items. Their recorded
  mismatches belong to its broad shell-behavior comparison; they are not an
  explicit demand in this PDF to implement all of Minishell.

## Confirmed findings

1. Norm formatting findings corrected after this review: wrapped long lines in
   src/usage_printer.c and src/main_bonus.c, removed whitespace on an empty
   line, and fixed the libft free-array helper header width. Full Norminette
   over src, includes, and libft now passes. Both builds pass.
2. Single/double quoted arguments and tab-separated command arguments do not
   match Bash. For input containing two `apple a1` lines, `grep 'apple a1'`
   piped into `wc -l` yields 0 instead of 2. Parsing only splits on literal spaces.
3. Deleted-executable rebuild fixed: if pipex is absent, Make forces the selected
   mode marker's link recipe to run. Tested for mandatory and bonus; repeating
   either build afterward does not relink.
4. Output-open failure behavior fixed: the parent reports the error but still
   launches the pipeline. Only the final child fails its unavailable output
   redirection; earlier commands can run. Tested with touch side effects in
   mandatory, multi-command bonus, and heredoc modes.
5. Child FD cleanup fixed: missing input/output and failed dup2 paths now close
   the pipeline descriptors before exit. Six fresh Valgrind filesystem-error
   cases show no unclosed project descriptors, heap errors, or leaks.
   Valgrind's own inherited log descriptor is excluded from the FD check.

## Function and submission checks

The compiler-generated memcpy/memset imports were corrected after this review.
Both printf struct initializers now call the project's ft_memset directly,
then restore their nonzero defaults. All four library copies were updated.
Twenty output/return-value comparisons against standard printf passed for each
copy. Mandatory and bonus Pipex builds pass; neither binary now imports memcpy
or memset. The changed helper passes Norminette. Runtime support symbols such
as __errno_location and __libc_start_main are separate.

The README now has named Instructions and Resources sections: build/run guidance
was moved near the top, and references and the AI disclosure were grouped together.
The owner still plans to review and revise the content.
The library folder is now libft with umbrella header ryker_libft.h and
original base header 0_libft/libft.h; Makefile paths, includes, and documentation were updated.
The supplied PDF does not prohibit Makefile wildcards. The earlier wildcard
concern came from another mirror and is withdrawn for this checklist.

## Results and limits

42 of 45 local build/runtime/heap checks now pass. The three remaining failures
are single quotes, double quotes, and tab separators, deliberately deferred by
the owner to Minishell. This ratio is not an evaluation score or a waiver of
shell-equivalence requirements.

Five- and twenty-pipe cases pass. Heredoc passes normal termination, EOF,
limiter without final newline, large input, and repeated append checks.
Additional checks confirm deleted-binary rebuild and no relink in both modes.
Six fresh Valgrind runs cover missing input and output-open failure in both
builds, including multi-command and heredoc side effects; no project FDs remain
open in the exiting processes, and no heap errors/leaks were reported.
Fresh FD logs: /tmp/pipex-fixed-fds-vacmroia/.

Full Pipex/embedded-libft Norm passes. This does not cover every fork/pipe/dup2,
signal, interrupted-wait, write, or unlink failure. No score or mandatory/bonus
approval is asserted. Peer understanding and the official submission are not tested.

Harnesses and logs: /tmp/pipex-path-audit/ . Test copy path is recorded in
/tmp/pipex-path-audit/eval-root . These temporary files may be removed by the OS.

## Individual checks

| Check | Result | Detail |
|---|---|---|
| clean build | PASS |  |
| mandatory no relink | PASS |  |
| rebuild deleted executable | PASS |  |
| argument count 0 | PASS |  |
| argument count 1 | PASS |  |
| argument count 2 | PASS |  |
| argument count 3 | PASS |  |
| argument count 5 | PASS |  |
| argument count 6 | PASS |  |
| subject example | PASS | status 0/0 |
| normal | PASS | status 0/0 |
| empty input | PASS | status 0/0 |
| large stream | PASS | status 0/0 |
| missing input | PASS | status 0/0 |
| unreadable input | PASS | status 0/0 |
| missing first command | PASS | status 0/0 |
| missing last command | PASS | status 127/127 |
| denied command | PASS | status 126/126 |
| directory command | PASS | status 126/126 |
| missing interpreter | PASS | status 126/126 |
| output directory | PASS | status 1/1 |
| output denied | PASS | status 1/1 |
| single quotes | FAIL | status 0/0 |
| double quotes | FAIL | status 0/0 |
| tab delimiter | FAIL | status 0/0 |
| early reader exit | PASS | status 0/0 |
| concurrent commands | PASS | 1.00s |
| first command runs despite output-open failure | PASS |  |
| bonus builds | PASS |  |
| bonus no relink | PASS |  |
| multiple pipes | PASS |  |
| heredoc normal | PASS |  |
| heredoc EOF | PASS |  |
| heredoc limiter without newline | PASS |  |
| heredoc large | PASS |  |
| clean rule | PASS |  |
| re rule | PASS |  |
| fclean rule | PASS |  |
| 5 pipes | PASS |  |
| 20 pipes | PASS |  |
| heredoc repeated append | PASS |  |
| Valgrind all input cat missing_93811 | PASS | 3 current-run process logs; status 127 |
| Valgrind all missing cat wc -l | PASS | 3 current-run process logs; status 0 |
| Valgrind all input missing_93811 missing_93812 | PASS | 3 current-run process logs; status 127 |
| Valgrind bonus here_doc END missing_93811 | PASS | 3 current-run process logs; status 127 |
