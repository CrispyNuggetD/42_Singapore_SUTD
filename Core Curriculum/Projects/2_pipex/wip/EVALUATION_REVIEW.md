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
3. Make does not recreate a deleted executable if obj/.mandatory still exists.
   Reproduction in the isolated copy: make; remove only pipex; make.
   Make reports success but the executable remains absent. Ordinary repeated
   builds do not relink, and clean/fclean/re work.
4. An output-open failure stops the whole pipeline before cmd1 starts.
   `pipex input "touch marker" cat directory` does not create marker;
   `< input touch marker | cat > directory` does. Both report status 1, so
   comparing status alone misses this difference.
5. With missing input, the first child exits before close_pipeline(). Valgrind
   reports three project descriptors still open: output and both pipe ends.
   The OS reclaims them on exit; this is explicit FD-cleanup debt, not a heap leak.
   Valgrind's own inherited log descriptor was excluded from this finding.

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

40 of 45 local build/runtime/heap checks passed. This ratio is not a score:
Norm, forbidden-function review, documentation, and FD cleanup are separate checks.
The five failures are deleted-binary rebuild, three parser cases, and first-command
side effects after output failure.

Five- and twenty-pipe cases passed. Heredoc passed normal termination, EOF,
limiter without final newline, large input, and repeated append checks.
Four Valgrind failure-path runs reported no heap errors or leaks; one exposed
the descriptor cleanup issue above. This does not cover every fork/pipe/dup2,
signal, interrupted-wait, write, or unlink failure. No score or mandatory/bonus
approval is asserted. Peer understanding and the official submission are not tested.

Harnesses and logs: /tmp/pipex-path-audit/ . Test copy path is recorded in
/tmp/pipex-path-audit/eval-root . These temporary files may be removed by the OS.

## Individual checks

| Check | Result | Detail |
|---|---|---|
| clean build | PASS |  |
| mandatory no relink | PASS |  |
| rebuild deleted executable | FAIL |  |
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
| first command runs despite output-open failure | FAIL |  |
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
| Valgrind all input cat missing_93811 | PASS | 3 process logs; status 127 |
| Valgrind all missing cat wc -l | PASS | 3 process logs; status 0 |
| Valgrind all input missing_93811 missing_93812 | PASS | 3 process logs; status 127 |
| Valgrind bonus here_doc END missing_93811 | PASS | 3 process logs; status 127 |
