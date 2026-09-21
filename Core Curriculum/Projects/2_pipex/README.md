*This project has been created as part of the 42 curriculum by hnah.*

# pipex — following file descriptors through a shell pipeline

## Description

Pipex recreates shell pipelines using Unix file descriptors and processes. The
mandatory program behaves like:

```sh
< infile cmd1 | cmd2 > outfile
```

and is invoked as:

```sh
./pipex infile "cmd1" "cmd2" outfile
```

The bonus accepts any number of commands and also supports a here-document:

```sh
./pipex infile "cmd1" "cmd2" ... "cmdN" outfile
./pipex here_doc LIMITER "cmd1" "cmd2" ... "cmdN" outfile
```

These correspond to:

```sh
< infile cmd1 | cmd2 | ... | cmdN > outfile
cmd1 << LIMITER | cmd2 | ... | cmdN >> outfile
```

## How the mandatory pipeline works

The parent opens the input and output files, creates one pipe, and forks two
children. The first child redirects the input file to standard input and the
pipe's write end to standard output. The second redirects the pipe's read end
to standard input and the output file to standard output. Each child then
resolves its command through `PATH` and calls `execve`.

The important detail for me was that `dup2()` does not move data itself. It
makes file descriptor 0 or 1 refer to another open file description. After the
redirections, an ordinary command can keep reading stdin and writing stdout
without knowing whether those descriptors lead to files, pipes, or the
terminal.

`fork()` produces the child processes, while `execve()` replaces each child's
program with the requested command. The parent must close its unused pipe ends;
otherwise a reader can wait forever because some process still owns a write
end. It then uses `waitpid()` to reap every child and returns the final
command's exit status, like a normal shell pipeline.

## Bonus: a rolling pipeline

My first question was whether all pipes had to be allocated in advance because
the commands run simultaneously. They do run concurrently, but the pipes do
not all have to exist before the first `fork()`.

The bonus keeps one `input_fd` and one `pipefd[2]`. For every command except
the last, the parent creates the next pipe and forks. The child connects
`input_fd` to stdin and the new pipe's write end to stdout. The parent closes
the descriptors it has finished with and carries the new read end forward as
the next `input_fd`:

```text
infile -> cmd1 -> pipe A -> cmd2 -> pipe B -> ... -> cmdN -> outfile
```

This is a **rolling pipe**. It uses a constant number of live pipe descriptors
in the parent instead of allocating an array for every pipe. Forking continues
without waiting, so all commands are still able to run at the same time. Only
after the whole pipeline exists does the parent wait for the children.

The `t_pipeline` struct is the parent's small piece of bookkeeping: current
input and output descriptors, the next pipe, command indexes, number of
children, and PID of the final child. Remembering the final PID lets the
program return that command's status while still reaping all other children.

## Bonus: `here_doc`, `<<`, and `>>`

I initially guessed that `>>` probably meant changing how the output file is
opened. That was right: mandatory output uses `O_TRUNC`, which clears an
existing file, while here-document mode uses `O_APPEND`, which preserves the
file and writes at its end.

For `<<`, the program reads stdin one line at a time until a line is exactly
the limiter. A GNL-style line normally contains its newline, so comparing only
the prefix would wrongly accept `STOPPING` for the limiter `STOP`. The check
therefore requires the next character to be either the string terminator, or a
newline immediately followed by the string terminator. The limiter line itself
is not written into the here-document.

### Why accept a limiter without a final newline?

After matching the limiter's characters, `is_limiter()` checks:

```c
if (line[length] == '\0')
    return (1);
```

Here, `length` is the limiter's length. For limiter `STOP`, this accepts the
string `"STOP"`, as distinct from `"STOP\n"`, which the next check handles.
The null terminator ends the C string; it is not an EOF byte read from stdin.
The line reader adds it when returning the final characters collected before
EOF. This check only accepts an exact match, not arbitrary input ending at EOF.

Pressing Enter in a terminal normally supplies a newline, but stdin is not
necessarily a keyboard: it can also be a file or a pipe. A producer can finish
immediately after writing `STOP`, without writing a final newline.

This is also a real Bash here-document case. The installed `man bash` explains
in its `-s` option that Bash can read commands from stdin, including through a
pipe. Its **Here Documents** section describes reading the body from the
current input source. A script containing `<<STOP` remains a here-document
when that script reaches Bash through a pipe; interactive typing is not a
requirement. See also the official manual's
[Bash invocation](https://www.gnu.org/software/bash/manual/html_node/Invoking-Bash.html)
and [redirection](https://www.gnu.org/software/bash/manual/html_node/Redirections.html)
sections.

The specific acceptance of a delimiter without a final newline was verified
experimentally with Bash, rather than inferred from an explicit statement
about this edge case in the manual:

```sh
# Exact delimiter followed by EOF, with no final newline: prints hello,
# with no warning. Adding a newline after STOP is accepted too.
printf 'cat <<STOP\nhello\nSTOP' | bash --noprofile --norc

# EOF without the delimiter: prints hello and warns that STOP was missing.
printf 'cat <<STOP\nhello\n' | bash --noprofile --norc
```

For a comparison matching the subject's `cmd << LIMITER | cmd1 >> file`
structure, build the bonus and run the following from the project root:

```sh
make bonus
test_dir=$(mktemp -d)
printf 'cat <<STOP | wc -l >> "%s/bash_out"\nhello\nSTOP' "$test_dir" \
    | bash --noprofile --norc
printf 'hello\nSTOP' \
    | ./pipex here_doc STOP "cat" "wc -l" "$test_dir/pipex_out"
diff -u "$test_dir/bash_out" "$test_dir/pipex_out"
```

Both output files should contain `1` followed by a newline; `diff` should print
nothing. The temporary directory keeps previous append output from affecting
the comparison. Bash receives shell syntax plus the body through stdin; Pipex
receives its command structure through arguments and only the body through
stdin. A plain `printf ... | cat` would merely be a pipeline, but the Bash
reference above actually parses and executes `<<STOP`.

The subject (version 5.0, bonus section, printed page 10) asks for equivalent
shell behavior and does not restrict input to a terminal. It does not name
Bash specifically or separately define this EOF edge case. This test supports
the delimiter check; it is not a claim that all Bash here-document features
are implemented or that the whole program has been validated.

### Why I chose a temporary file

We discussed two ways to turn the collected text into the first command's
stdin:

- A pipe can stream the lines directly, but a pipe has finite capacity. Filling
  it before a reader exists can block. It is safe when the reader and writer
  run concurrently, but that makes process ordering and cleanup less simple.
- A temporary file can be filled completely, reopened for reading, unlinked,
  and then treated exactly like the normal infile descriptor. It also handles
  input larger than a pipe buffer without a concurrent reader.

I chose the temporary-file method because it follows the model that was easier
for me to reason about and is close to zsh's implementation. The program uses
`O_EXCL` to avoid taking over an existing temporary path, reopens the completed
file for reading, and immediately calls `unlink()`. The descriptor remains
usable, but the pathname is removed and the operating system can reclaim the
file after the descriptor closes.

This is also useful preparation for Minishell. Both
[zsh](https://zsh.sourceforge.io/Doc/Release/Redirection.html) and
[Bash](https://www.gnu.org/software/bash/manual/html_node/Redirections.html)
first define the same visible rule: read until a line containing only the
delimiter, then provide the preceding text as stdin. Their internal storage is
not a shell-language guarantee. zsh uses temporary-file-backed handling;
modern Bash implementations may use a pipe for smaller documents and temporary
storage when needed. Minishell needs to reproduce the behavior, not blindly
copy one shell's private mechanism.

### Shared unique-file creation

`prepare_heredoc()` calls `ryker_ft_create_open_unique_file()` with the full
`/tmp/pipex_here_doc_` prefix. The helper creates and opens a new write-only file,
returns its allocated pathname, retries collisions, and clears the output path
on failure. Its caller owns close/unlink/free. Main Ryker libft retains a separate
one-level directory-creation wrapper as WIP for study. It is excluded from normal
builds and entirely omitted from this Pipex copy, including its declaration.
It has not yet been reviewed and understood by the author; passing tests alone
do not make it ready for use. `mkdir()` and `stat()` are also outside Pipex's
allowed-function list. See
[the file_unique API](ryker_libft/ryker_ft/file_unique/README.md) for ownership,
error codes, permissions, and limitations.

### What does `unlink()` actually remove?

The error cleanup in `heredoc_bonus.c` contains:

```c
if (tmp_filename)
{
    unlink(tmp_filename);
    free(tmp_filename);
}
```

`tmp_filename` points to an allocated C string such as
`"/tmp/pipex_here_doc_0"`. The `if` checks that this pointer is not `NULL`;
it does **not** check whether the file exists.

`unlink(tmp_filename)` removes that pathname from the filesystem. The name
comes from removing a **link** between a directory entry and a file. A file
can have several names (hard links), so removing one name does not necessarily
remove the underlying file. Its storage can be reclaimed once its last hard
link is gone and no process still has it open.

These three cleanup operations manage different resources:

| Operation | Resource released | What it does not do |
|---|---|---|
| `close_and_void_fd(fd)` | Closes the descriptor and sets the caller's integer to `-1` | Does not remove the pathname |
| `unlink(tmp_filename)` | Removes the filesystem name | Does not close existing descriptors or free the C string |
| `free(tmp_filename)` | Releases the allocated memory holding the pathname string | Does not remove the file or close it |

The pathname string must remain valid until `unlink()` has used it, which is
why `free()` comes afterward. `free()` does not reset the pointer to `NULL`;
here, `temp_error()` immediately returns and the caller also returns, so that
pointer is not used again.

On the successful here-document path, the order is slightly different:

```text
write the body -> close write_fd -> open read_fd -> unlink the pathname
                                               -> free the pathname string
```

The already-open `read_fd` still works after unlinking! The first command can
read the collected text through it, although opening the old pathname again
would fail unless someone creates a new file there. After the last open
descriptor is closed, the file's storage can be reclaimed, assuming no other
hard links exist. This lets the pipeline use the temporary data without
keeping a named temporary file around.

`unlink()` returns `0` on success or `-1` on failure and sets `errno`.
The current cleanup ignores its return value, so removal is attempted but
not guaranteed: a failed unlink can leave a temporary file behind.

### Reusing GNL with caller-owned state

Heredoc now uses `ryker_ft_get_next_line()` from
[`ryker_libft/ryker_ft/gnl_status`](ryker_libft/ryker_ft/gnl_status/README.md).
Its header is `ryker_ft_get_next_line.h`, exposed through the library umbrella.

```c
void            ryker_ft_gnl_init(t_gnl_info *gnl, int fd);
t_gnl_result    ryker_ft_get_next_line(t_gnl_info *gnl, char **line);
void            ryker_ft_gnl_cleanup(t_gnl_info *gnl);
```

The enum distinguishes `GNL_LINE` (1), `GNL_EOF` (0), and `GNL_ERROR` (-1).
The caller owns `t_gnl_info`, which stores the stash, borrowed FD, and EOF flag.
Returned lines are freed separately. Cleanup frees the stash without closing
the FD, including when stopping early at the limiter or when writing fails.
This replaces both the byte-at-a-time local implementation and the local prototype.

The original GNL remains available. This extension shares its helpers rather
than calling its entry point, whose private static stash and ambiguous `NULL`
return cannot provide the extra control. The linked component README explains
the full call sequence, why `nl` includes the newline, and ownership rules.
Buffered reads can consume text after the limiter; cleanup discards it rather
than returning it to stdin. Future Minishell reuse must account for this if
command input continues from the same stream.

## Output permissions: `0644`, `0666`, and `umask`

The current code requests `0644` when creating an output file. For the shared
output-opening helper, I plan to use `0666` to match ordinary shell redirection
and make the helper suitable for reuse in Minishell.

The mode passed to `open()` is a starting permission set, not necessarily the
file's final permissions. In the usual case without a default directory ACL,
the process's `umask` removes permissions: `mode & ~umask`.

| Requested mode | `umask` | Result |
|---|---|---|
| `0644` | `0022` | `0644` |
| `0666` | `0022` | `0644` |
| `0644` | `0002` | `0644` |
| `0666` | `0002` | `0664` |

Hard-coding `0644` prevents group/others write access even when the user's
`umask` would permit it. Requesting `0666` lets that mask determine which read
and write permissions to remove; it does not request execute permission.
With `umask 0077`, either mode produces `0600`.

This creation mode only applies when `O_CREAT` creates a new file. Appending
to or truncating an existing file does not reset its permission bits.

## Execution errors and process exit statuses

Handling a failed command involves three separate responsibilities: preserving
why execution failed, reporting that failure, and choosing the result returned
to the parent. Keeping these responsibilities separate is necessary for
shell-compatible error handling.

### Two numbering systems

Although both fit in an `int`, `errno` and a process exit status represent
different information:

| Value | Meaning | Consumer |
|---|---|---|
| `errno`, optionally copied into `saved_errno` | Why a particular operation failed | Error-handling code, `perror()`, or `strerror()` |
| Process exit status | The result a process reports when it finishes | Its parent through `waitpid()`, or the shell through `$?` |

For example, `EACCES` means permission denied and has value 13 on the local
Linux system. After an execution failure with that error, `exit(saved_errno)`
would report status 13. It would not produce the shell status 126 expected for
that execution failure. The program must select the appropriate exit status.

Error comparisons should use symbolic names such as `EACCES` and `ENOENT`,
because their numeric values can differ across systems. Bash's exit-status
conventions assign 127 to a command not found and 126 to a command found but
unable to execute. See the
[Bash exit-status documentation](https://www.gnu.org/s/bash/manual/html_node/Exit-Status.html).
An executed program can also choose its own status, so observing 126 alone
does not identify the specific operation that failed.

In the Unix process model used here, the parent obtains the low eight bits of
the value passed to `exit()` as the normal exit status. Exiting with an arbitrary
`errno` value is therefore not a general mechanism for transmitting the original
error to the parent.

### Preserving the cause before handling the failure

A successful `execve()` replaces the process image and never returns. If it
returns with failure, `errno` describes the cause at that point. Copying it
immediately preserves that information for subsequent classification and
cleanup:

```text
execve fails -> save errno -> classify failure -> report original error
             -> free allocations -> exit with the selected shell status
```

The same principle applies to `open()`, `read()`, `write()`, and `fork()`.
Saving the error is useful whenever intervening operations could overwrite it
before it has been fully handled. It is not required merely because a
particular system call was used.

For an immediate report followed by termination, a helper such as
`exit_perror("malloc", 1)` can use the current `errno`, provided the allocation
error has not been overwritten. Here, `1` is the chosen process exit status.
For delayed reporting, restore the saved value to `errno` before `perror()`,
or obtain its message with `strerror(saved_errno)`. Neither reporting function
chooses the process's exit status.

Only interpret `errno` when the operation's return value indicates failure.
A successful operation does not necessarily clear an earlier error.

### Why execution failures need classification

Reporting immediately after `execve()` and then always exiting with 126
preserves the diagnostic, but loses distinctions needed for shell behavior.
The following focused tests illustrate those distinctions using Bash installed
on the local 42 computer:

| Attempt | Execution error observed | Local Bash exit status |
|---|---|---|
| Missing explicit executable path | `ENOENT` | 127 |
| Existing file without execute permission | `EACCES` | 126 |
| Directory used as a command | `EACCES` | 126 |
| Existing executable script with a missing `#!` interpreter | `ENOENT` | 126 |
| Path with a non-directory component | `ENOTDIR` | 126 |

In particular, `ENOENT` does not prove that the executable file itself is
missing: a script may exist while the interpreter named by its `#!` line does
not. Mapping every `ENOENT` to 127 would not reproduce these local Bash results.
Similarly, a directory can produce `EACCES` from execution while Bash reports
the more specific diagnostic "Is a directory". Classification may therefore
require context in addition to the saved error code.

These results describe the tested reference shell; edge-case behavior can vary
between shells and versions. The scoped execution-error classification is implemented and tested against
local Bash 5.1.16. It preserves the original system diagnostic; custom Bash
wording and script fallback are outside this change. Its post-failure existence
check can race with filesystem changes. The first PATH-search improvement is
now implemented below; remaining work is tracked in [wip/STATUS.md](wip/STATUS.md).
The distinction guiding that work is that the error code describes the failed
operation, while the exit status communicates the program's chosen outcome.

## PATH search: trying later candidates (2026-09-21)

The main change implements **behavior 1 of the six-part PATH study plan**:
try a later executable when an earlier candidate is not executable. Supporting
it also required limited permission-error retention and a stop/continue rule.
It would therefore be inaccurate to say that behaviors 2 and 3 are untouched,
or that all six behaviors are complete. These six items are subdivisions of
PATH-search task (3) in the older checkpoint, not six separate subject tasks.

### What changed from the previous implementation?

Previously, `search_directories()` returned the first candidate for which
`access(candidate, F_OK)` succeeded. `F_OK` checks existence, not executable
permission. `execute_command()` then attempted that single path and exited if
execution failed. An earlier existing file could hide a later usable command.

For example, with PATH `/first:/second` and command `hello world`:

| Candidate | State | Previous behavior | Current behavior |
|---|---|---|---|
| `/first/hello` | Exists without execute permission | Select it, fail execution, exit 126 | Remember the permission failure and continue |
| `/second/hello` | Executable program | Never reached | Execute it with `world` as its argument |

Execution attempts now happen inside the directory search. We did not merely
replace `F_OK` with `X_OK`: an execute-permission check alone cannot establish
that loading the program will succeed. The existing `F_OK` precheck remains,
with the limitations described below.

### Follow one command through the functions

1. `execute_command()` splits `command_str` into `args`. For `hello world`,
   `args[0]` is `hello`, followed by the argument `world` and a NULL terminator.
2. It passes the whole argument array to `resolve_path()`. The search needs
   both `args` and `envp` because it now calls `execve()`.
3. If `args[0]` contains `/`, `resolve_path()` copies that explicit path and
   returns `PATH_READY`. The caller executes it once without searching PATH.
4. Otherwise, `resolve_path()` retrieves the PATH value into its local `path`,
   splits it into `directories`, and calls `search_directories()`.
5. The loop allocates one `candidate` by joining a directory with `args[0]`.
   `try_path_candidate()` checks existence and attempts execution.
6. Successful `execve()` replaces the child process image. It never returns
   to the helper, loop, or caller. The requested program starts running in
   that child; it is not another function inside Pipex.
7. Failed attempts either permit the next directory or end the search. If
   the search returns, the caller handles allocation failure, command not
   found, or the retained execution error.

If the executed program later exits with a nonzero status, that does not resume
PATH searching. Execution already succeeded; the parent receives the program's
exit status through its normal waiting logic.

The name `resolve_path()` still understates its responsibility: for a plain
command name it attempts execution, whereas for an explicit path it prepares
the path for the caller. This remains a possible future readability refactor.

### Why keep both `candidate` and `command_path`?

`candidate` is the path being tried now. During PATH search, `*command_path`
retains the first candidate that reached `execve()` and failed with `EACCES`.
For example, it can retain `/first/hello` while `candidate` holds
`/second/hello`.

The `char **command_path` parameter is an output pointer to one `char *`
variable in the caller. It is not an array of found paths. Assigning a
candidate to `*command_path` transfers responsibility for that allocated string;
it does not duplicate the string. The helper must not free a retained string
while the caller still needs it.

Keeping the failed path lets the existing final error handler identify the
file. It is a design choice: another implementation could free each candidate
immediately and retain only error information, with a different reporting API.
The loop does not need an array containing every candidate.

### Return values now describe distinct outcomes

The old `PATH_FOUND` name was removed because it had acquired two meanings:
a path ready to execute, and an execution failure ready to report.

| Result | Meaning | Where it is used |
|---|---|---|
| `PATH_READY` | An explicit path is prepared; execution has not been attempted | `resolve_path()` to `execute_command()` |
| `EXEC_FAILED` | Stop searching and report the retained path and error | Candidate helper, search, and caller |
| `SEARCH_CONTINUE` | Try another directory | Candidate helper to the loop only |
| `PATH_NOT_FOUND` | Overall lookup has no usable or retained denied candidate | Lookup to caller; exits 127 |
| `PATH_ERROR` | Lookup allocation failed | Lookup to caller; exits 1 |

Successful execution returns none of these. At the loop's comparison with
`EXEC_FAILED`, reaching the comparison means the helper returned and did not
successfully execute the program.

### Current retry and reporting rules

- If the `F_OK` precheck fails, free this candidate and continue.
- If `execve()` fails with `EACCES`, retain the first denied candidate and
  continue. Free later denied candidates rather than replacing the first.
- If `execve()` fails with another error, save that error, replace any retained
  denied path with this candidate, and return `EXEC_FAILED` immediately.
- If directories run out and a denied path is retained, restore `EACCES` and
  return `EXEC_FAILED`. The final handler reports permission denied and exits 126.
- If directories run out with no retained path, return `PATH_NOT_FOUND`.
- If candidate allocation fails, free any retained path, clear the output
  pointer, and return `PATH_ERROR`; this must not become command not found.

The existing `exec_failure()` selects an exit status, restores the original
error for `perror()`, frees the retained
path and arguments, and exits. It now uses `perror(path)` instead of
`perror(args[0])`, so an execution error identifies the failed file. The
command-not-found diagnostic still names the command.

The candidate helper is in the new `src/path_candidate.c`; the Makefile includes
it in both builds. `includes/pipex.h` declares the helper, the updated resolver
parameters, and the clearer result names.

### In what sense is this closer to Bash?

Local Bash experiments showed these specific behaviors:

- Three non-executable candidates followed by an executable fourth candidate:
  the fourth ran, and stderr was empty. Earlier skipped candidates were not
  reported.
- Two non-executable candidates with no usable later candidate: Bash printed
  one permission-denied diagnostic naming the first candidate and exited 126.

Our implementation now supports those behaviors: retries are silent, a later
success produces no earlier permission diagnostic, and an exhausted search can
report the first denied path once. It also preserves the first usable candidate
rather than continuing after successful execution.

These are observed behavioral matches, not a claim to implement Bash's internal
search algorithm or all its diagnostics. The earlier failure-classification
checks used Bash 5.1.16; the later candidate-reporting experiments used the
installed `/bin/bash`. Diagnostic prefixes and some error wording still differ.

After reapplying onto upstream commit `24f497d`, mandatory and bonus builds each
passed nine focused checks: denied then usable, directory then usable, first
usable wins, two denied paths, denied then missing, explicit denied path, all
missing, normal explicit execution, and missing explicit execution. The two-denied
check required exactly one diagnostic naming the first path. Changed C files and
the header passed Norm. These checks are not a full leak, FD, or failure audit.

### What remains from the six-part study plan?

| Behavior | Current scope and remaining work |
|---|---|
| 1. Try later candidates | Implemented for permission-denied execution attempts, including an earlier directory; later usable commands can run. |
| 2. Remember permission failures | Partly implemented: retain the first `execve()` EACCES. Permission failures from the F_OK precheck are still discarded. |
| 3. Decide when to stop | A limited policy exists: retry EACCES, stop on other execve errors. Broader error cases and reference-shell comparisons remain. |
| 4. Preserve empty PATH entries | Not implemented. `ft_split()` still drops leading, trailing, and consecutive empty entries that should represent the current directory. |
| 5. Define empty/missing PATH behavior | Unchanged: both currently lead to not found for a plain command name. Deliberate reference-shell checks remain. |
| 6. Preserve allocation handling and cleanup | Existing allocation errors remain separate from not found, and discarded/retained paths have cleanup paths. The changed search still needs allocation-failure injection and a full ownership audit. |

In particular, an inaccessible directory may make the F_OK precheck fail with
permission denied, which is currently treated like any other failed precheck.
Filesystem changes between checking and executing can also change the result.
There is no ENOEXEC shell-script fallback, and command splitting still does not
implement shell quoting. Full Bash equivalence is outside this study step.

## Build

```sh
make          # mandatory executable
make bonus    # bonus executable
make clean
make fclean
make re
```

Mandatory and bonus use separate entry points. The bonus implementation is in
`*_bonus.c` and `*_bonus.h` files as required by the subject. Build markers
also remember which version currently produced `pipex`, so repeating the same
build does not relink and switching modes rebuilds the correct executable.

## Use of AI

AI helped me scaffold the initial project in the style of my FdF repository,
call and edit functions under my direction, test edge cases, and check the code
against the subject and Norm. More importantly, I used it for a learning
discussion: why concurrent commands do not require every pipe to be stored,
why an unread here-document pipe can fill and block, how exact limiter matching
works, and how real shells can choose different internal storage while keeping
the same behavior.

Ideas recorded above include questions and deductions I brought into that
discussion, such as recognising that `>>` implies append mode and asking how
simultaneous processes affect pipe creation. This was not a request to "vibe
code" a submission I cannot explain. AI performed much of the mechanical
editing and function calling, but I will read, understand, test, and edit every
part before submission. I will not present code I do not understand as my own.

## Work in progress

See [wip/STATUS.md](wip/STATUS.md) for completed work, remaining shell-correctness
tasks, validation limits, and the directory wrapper's WIP restrictions.
