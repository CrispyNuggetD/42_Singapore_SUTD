*This project has been created as part of the 42 curriculum by hnah.*

<a id="top"></a>

# pipex — following file descriptors through a shell pipeline

<a id="at-a-glance"></a>

## At a glance

✅ = implemented. ❌ = not implemented. This describes the current feature set;
it is not an awarded evaluation score.

**Local evaluation: 45/45 checks passed.**
[Jump to the checks and reproducible command examples](#evaluation-checks).

| Status | Feature | Current behavior |
|---|---|---|
| ✅ | Mandatory pipeline | Two commands, input redirection, output creation/truncation, concurrent execution. |
| ✅ | Bonus: multiple pipes | Multiple commands run concurrently; a rolling pipeline keeps the parent’s pipe-FD count constant. |
| ✅ | Bonus: `here_doc` and append | Exact limiter matching, append output, EOF handling, and a limiter without a final newline; temporary-file storage supports large input. |
| ✅ | Command lookup | PATH search and explicit relative/absolute command paths. |
| ✅ | Retry permission-denied candidates | Later usable commands can run; retain the first denied path if the search fails, including denial from `access()`. |
| ✅ | Empty PATH entries | Leading, trailing, and consecutive `:` entries search the current directory. Empty and unset PATH do too. |
| ✅ | Error reporting and status | Returns the last command’s status and distinguishes command-not-found from execution failures. |
| ✅ | Output-open failure | Earlier commands still run when the final output cannot open; final child exits with failure. |
| ✅ | Build and Norm | Separate mandatory/bonus builds, no unnecessary relinking, deleted-binary rebuild, and Norm-compliant Pipex/bundled-libft sources. |
| ✅ | [Command argument parsing](#command-argument-parsing) | Spaces/tabs, single/double quotes, empty quoted arguments, adjacent fragments, and unmatched-quote detection. |
| ❌ | Full shell language / fallback | No backslash escaping, variable expansion, globbing, command substitution, builtins, or `ENOEXEC` shell-script fallback. |
| ❌ | Interrupted-wait and unlink recovery | Interrupted waits are not retried systematically; temporary-file unlink failures are not reported. |

[↑ Back to top](#top)

<a id="design-choices-and-edge-cases"></a>

## Design choices and edge cases

These are implementation choices and supported edge cases, separate from the
mandatory and bonus requirements. Follow a link for the explanation.

| Status | Choice | Why it matters |
|---|---|---|
| ✅ | [Temporary-file heredoc](#why-i-chose-a-temporary-file) | Collect large input before starting readers without filling a pipe and blocking; requires filesystem cleanup. |
| ✅ | [Limiter without final newline](#why-accept-a-limiter-without-a-final-newline) | Accept an exact limiter when a file or pipe ends immediately after it. |
| ✅ | [Exclusive temporary-file creation](#shared-unique-file-creation) | Retry name collisions without overwriting an existing file. |
| ✅ | [Unlink after reopening](#what-does-unlink-actually-remove) | Remove the pathname while the open descriptor keeps the heredoc data usable. |
| ✅ | [GNL with explicit state](#reusing-gnl-with-caller-owned-state) | Distinguish a line, EOF, and an error; release buffered input when collection stops. |
| ✅ | [Rolling pipeline](#bonus-a-rolling-pipeline) | Keep the parent's pipe-FD usage constant as commands are added. |
| ✅ | [Allocation ownership](#path-allocation-and-ownership) | Free partial PATH results and retain only the path needed for error reporting. |
| ✅ | [Child FD cleanup](#file-open-errors-and-child-cleanup) | Close inherited pipeline descriptors before exiting on redirection failure. |
| ✅ | Printf struct initialization | Use our own `ft_memset`, avoiding compiler-generated external memory calls in the current build. |

[↑ Back to top](#top)

<a id="contents"></a>

## Contents

- [At a glance](#at-a-glance)
- [Design choices and edge cases](#design-choices-and-edge-cases)
- [Description](#description)
- [Instructions](#instructions)
- [How the mandatory pipeline works](#how-the-mandatory-pipeline-works)
- [Bonus: a rolling pipeline](#bonus-a-rolling-pipeline)
- [Bonus: `here_doc`, `<<`, and `>>`](#bonus-here_doc-and)
- [Output permissions: `0644`, `0666`, and `umask`](#output-permissions-0644-0666-and-umask)
- [Execution errors and process exit statuses](#execution-errors-and-process-exit-statuses)
- [PATH search: trying later candidates](#path-search-trying-later-candidates)
- [Command argument parsing](#command-argument-parsing)
- [Evaluation: 45 checks and examples](#evaluation-checks)
- [Reference comparison: Darren's Pipex](#reference-comparison-darrens-pipex)
- [Resources](#resources)

[↑ Back to top](#top)

<a id="description"></a>

## Description

Pipex explores Unix pipes, file descriptors, and process creation in C. It
connects commands with `pipe()`, starts them with `fork()` and `execve()`, and
redirects their input and output with `dup2()`.

The mandatory version connects two commands between an input and output file.
The bonus adds multiple pipes and a here-document mode with output append.
The implementation notes below explain the design and its current limitations.

[↑ Back to top](#top)

<a id="instructions"></a>

## Instructions

Run the following commands from this project's directory. Building requires
`make`, a C compiler available as `cc`, and `ar` for the bundled `libft` archive.
The Makefile builds the library automatically; no separate installation is needed.

[↑ Back to top](#top)

<a id="build-and-clean"></a>

### Build and clean

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
If `pipex` is deleted while a marker remains, Make forces the selected mode to
link again; a marker alone no longer counts as a completed build.

[↑ Back to top](#top)

<a id="run-the-mandatory-version"></a>

### Run the mandatory version

```sh
make
./pipex infile "grep a1" "wc -w" outfile
```

This connects the commands like the shell pipeline:

```sh
< infile grep a1 | wc -w > outfile
```

The input file must exist and be readable. The output file is created if needed;
an existing output file is truncated.

[↑ Back to top](#top)

<a id="run-the-bonus-version"></a>

### Run the bonus version

```sh
make bonus
./pipex infile "cat" "grep a1" "wc -l" outfile
./pipex here_doc STOP "cat" "wc -l" outfile
```

For `here_doc`, enter input lines followed by `STOP` on its own line. That
limiter ends collection and is excluded from the data sent to the commands.
The result is appended to `outfile`, preserving its existing contents.

Both builds produce `./pipex`; use `make` or `make bonus` to select the version.
See [command argument parsing](#command-argument-parsing) for supported quoting, and
[the current limitations](#current-limitations) for other boundaries.

[↑ Back to top](#top)

<a id="how-the-mandatory-pipeline-works"></a>

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
end. It then uses `waitpid()` for the final child and `wait()` for the others, returning the final
command's exit status, like a normal shell pipeline.

[↑ Back to top](#top)

<a id="file-open-errors-and-child-cleanup"></a>

### File-open errors and child cleanup

An input or output open error is reported by the parent, but a normal pipeline
still starts. Missing input prevents the first command from executing; later
commands can read EOF. Missing output prevents only the last command from
executing, so earlier commands can still perform side effects. The final child's
status determines the pipeline result. Heredoc collection failure still aborts
setup; an output-open failure after collection follows the same final-child rule.

For example, assume `infile` is readable, the current directory is writable,
and opening `unwritable_file` for output fails:

```sh
< infile touch marker | cat > unwritable_file
./pipex infile "touch marker" "cat" unwritable_file
```

In both cases, `touch` still creates `marker` even though the final command's
output redirection fails. The pipeline reports the output error and exits with
status 1. The commands run concurrently, so the error may appear before `marker`
is created.

That is why the output-open check calls `perror(argv[argc - 1])` without
returning a setup failure. Returning there would make the parent abort before
forking either command. Instead, `output_fd` remains `-1`, and the final child
closes its descriptors and exits without executing `cat`; earlier children
can still execute. Use separate fresh marker names when comparing the two runs
so a marker left by the first run does not hide a failure in the second.

Each child selects its output descriptor (next pipe or final file), checks its
input/output, and closes all pipeline descriptors before exiting on a missing
redirection or dup2 failure. This also closes inherited output and pipe ends in
the first child when input could not be opened.

[↑ Back to top](#top)

<a id="bonus-a-rolling-pipeline"></a>

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

[↑ Back to top](#top)

<a id="bonus-here_doc-and"></a>

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

[↑ Back to top](#top)

<a id="why-accept-a-limiter-without-a-final-newline"></a>

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

For example, the producer below ends immediately after the limiter:

```sh
printf 'hello\nSTOP' | ./pipex here_doc STOP "cat" "wc -l" outfile
```

Pipex excludes `STOP` from the collected input and appends `1` followed by a
newline to `outfile`. If EOF arrives without a limiter, it passes the text
collected so far to the pipeline. This accepts non-interactive input as well as
terminal input; it does not implement every shell here-document feature.

[↑ Back to top](#top)

<a id="why-i-chose-a-temporary-file"></a>

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

[↑ Back to top](#top)

<a id="shared-unique-file-creation"></a>

### Shared unique-file creation

`prepare_heredoc()` calls `ryker_ft_create_open_unique_file()` with the full
`/tmp/pipex_here_doc_` prefix. The helper creates and opens a new write-only file,
returns its allocated pathname, retries collisions, and clears the output path
on failure. Its caller owns close/unlink/free. The helper uses exclusive file
creation to avoid overwriting an existing pathname, and retries name collisions.
It creates a file directly in `/tmp`; no directory-creation helper is part of
this Pipex implementation. See
[the file_unique API](libft/ryker_ft/file_unique/README.md) for ownership,
error codes, permissions, and limitations.

[↑ Back to top](#top)

<a id="what-does-unlink-actually-remove"></a>

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

[↑ Back to top](#top)

<a id="reusing-gnl-with-caller-owned-state"></a>

### Reusing GNL with caller-owned state

Heredoc uses `ryker_ft_get_next_line()` from
[`libft/ryker_ft/gnl_status`](libft/ryker_ft/gnl_status/README.md).
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

[↑ Back to top](#top)

<a id="output-permissions-0644-0666-and-umask"></a>

## Output permissions: `0644`, `0666`, and `umask`

The output-opening helper requests `0666` when creating an output file,
letting the process umask determine the final permissions, like ordinary shell
redirection. The table compares this with a fixed `0644` request.

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

[↑ Back to top](#top)

<a id="execution-errors-and-process-exit-statuses"></a>

## Execution errors and process exit statuses

Handling a failed command involves three separate responsibilities: preserving
why execution failed, reporting that failure, and choosing the result returned
to the parent. Keeping these responsibilities separate is necessary for
shell-compatible error handling.

[↑ Back to top](#top)

<a id="two-numbering-systems"></a>

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

[↑ Back to top](#top)

<a id="preserving-the-cause-before-handling-the-failure"></a>

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

[↑ Back to top](#top)

<a id="why-execution-failures-need-classification"></a>

### Why execution failures need classification

Reporting immediately after `execve()` and then always exiting with 126
preserves the diagnostic, but loses distinctions needed for shell behavior.
The execution-error handler uses the following classification:

| Attempt | Execution error | Pipex exit status |
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

Pipex preserves the original system diagnostic. Custom Bash wording and
`ENOEXEC` script fallback are not implemented. The existence check used to
classify an execution error can race with filesystem changes.

[↑ Back to top](#top)

<a id="path-search-trying-later-candidates"></a>

## PATH search: trying later candidates

PATH lookup attempts execution in directory order. An earlier candidate that
cannot be executed because of permission denial does not hide a later usable
command.

For example, with PATH `/first:/second` and command `hello world`:

| Candidate | State | Action |
|---|---|---|
| `/first/hello` | Exists without execute permission | Remember the permission failure and continue. |
| `/second/hello` | Executable program | Execute it with `world` as its argument. |

`F_OK` checks existence, not executable permission. Even an `X_OK` check would
not establish that the program can be loaded, so the search attempts `execve()`
and handles its actual result.

[↑ Back to top](#top)

<a id="follow-one-command-through-the-functions"></a>

### Follow one command through the functions

1. `execute_command()` splits `command_str` into `args`. For `hello world`,
   `args[0]` is `hello`, followed by the argument `world` and a NULL terminator.
2. It passes the whole argument array to `attempt_possible_candidates()`. The search needs
   both `args` and `envp` because it calls `execve()`.
3. If `args[0]` contains `/`, `attempt_possible_candidates()` copies that explicit path and
   returns `DIRECT_PATH_SUPPLIED`. The caller executes it once without searching PATH.
4. Otherwise, `attempt_possible_candidates()` retrieves PATH into `path`, using
   `""` if PATH is unset. `split_path_empty_as_dot()` creates `directories`,
   converting every empty entry to `"."`, before `search_directories()` runs.
5. The loop allocates one `candidate_command` by joining a directory with `args[0]`.
   `attempt_one_candidate()` checks existence and attempts execution.
6. Successful `execve()` replaces the child process image. It never returns
   to the helper, loop, or caller. The requested program starts running in
   that child; it is not another function inside Pipex.
7. Failed attempts either permit the next directory or end the search. If
   the search returns, the caller handles allocation failure, command not
   found, or the retained execution error.

If the executed program later exits with a nonzero status, that does not resume
PATH searching. Execution already succeeded; the parent receives the program's
exit status through its normal waiting logic.

`attempt_possible_candidates()` attempts execution for a plain command name.
For an explicit path, it prepares the path for execution by the caller.

[↑ Back to top](#top)

<a id="why-keep-both-candidate_command-and-exec_fail_path"></a>

### Why keep both `candidate_command` and `exec_fail_path`?

`candidate_command` is the path being tried now. During PATH search, `*exec_fail_path`
retains the first candidate denied with `EACCES` by either `access()` or `execve()`.
For example, it can retain `/first/hello` while `candidate_command` holds
`/second/hello`.

The `char **exec_fail_path` parameter is an output pointer to one `char *`
variable in the caller. It is not an array of found paths. Assigning a
candidate to `*exec_fail_path` transfers responsibility for that allocated string;
it does not duplicate the string. The helper must not free a retained string
while the caller still needs it.

Keeping the failed path lets the existing final error handler identify the
file. It is a design choice: another implementation could free each candidate
immediately and retain only error information, with a different reporting API.
The loop does not need an array containing every candidate.

[↑ Back to top](#top)

<a id="return-values-now-describe-distinct-outcomes"></a>

### Return values now describe distinct outcomes

The result distinguishes a path ready to execute from an execution failure
ready to report:

| Result | Meaning | Where it is used |
|---|---|---|
| `DIRECT_PATH_SUPPLIED` | An explicit path is prepared; execution has not been attempted | `attempt_possible_candidates()` to `execute_command()` |
| `EXEC_FAILED` | Stop searching and report the retained path and error | Candidate helper, search, and caller |
| `SEARCH_CONTINUE` | Try another directory | Candidate helper to the loop only |
| `COMMAND_NOT_FOUND` | Overall lookup has no usable or retained denied candidate | Lookup to caller; exits 127 |
| `ALLOCATION_FAILED` | Lookup allocation failed | Lookup to caller; exits 1 |

Successful execution returns none of these. At the loop's comparison with
`EXEC_FAILED`, reaching the comparison means the helper returned and did not
successfully execute the program.

[↑ Back to top](#top)

<a id="current-retry-and-reporting-rules"></a>

### Current retry and reporting rules

- If the `F_OK` precheck fails with an error other than `EACCES`, free this
  candidate and continue.
- If either the precheck or `execve()` fails with `EACCES`, retain the first denied candidate and
  continue. Free later denied candidates rather than replacing the first.
- If `execve()` fails with another error, save that error, replace any retained
  denied path with this candidate, and return `EXEC_FAILED` immediately.
- If directories run out and a denied path is retained, restore `EACCES` and
  return `EXEC_FAILED`. The final handler reports permission denied and exits 126.
- If directories run out with no retained path, return `COMMAND_NOT_FOUND`.
- If candidate allocation fails, free any retained path, clear the output
  pointer, and return `ALLOCATION_FAILED`; this must not become command not found.

`exec_failure_error()` selects an exit status, restores the original error for
`perror()`, frees the retained path and arguments, and exits. `perror(path)`
identifies the failed file; the command-not-found diagnostic names the command.

The candidate helper is in `src/path_candidate.c`; the Makefile includes
it in both builds. `includes/pipex.h` declares the helper, resolver parameters,
and result names.

[↑ Back to top](#top)

<a id="retry-and-reporting-behavior"></a>

### Retry and reporting behavior

Retries are silent. If a later command executes successfully, no diagnostic is
printed for earlier denied candidates. If none succeeds, the first retained
permission-denied path is reported once. Successful execution ends the search,
even if that program later exits with a failure status.

[↑ Back to top](#top)

<a id="current-limitations"></a>

### Current limitations

The search retries `EACCES` but stops on other `execve()` errors. If a candidate
disappears between `access()` and `execve()`, or its interpreter is missing,
that execution error stops the search even if a later candidate could work.
There is no `ENOEXEC` shell-script fallback or full shell-language parser.

Interrupted waits are not retried systematically, and temporary-file unlink
failures are currently ignored. These are limitations of the current code;
the feature checklist is not a claim of complete shell equivalence.

[↑ Back to top](#top)

<a id="command-argument-parsing"></a>

### Command argument parsing

`split_command_args()` in `src/argument_parser.c` returns a NULL-terminated
argument array. Spaces and tabs separate arguments only outside quotes.
Matching single/double quotes are removed; the other quote type stays literal
inside a quoted region. Adjacent fragments form one argument, and empty quotes
produce an empty argument.

| Command string received by Pipex | Resulting arguments |
|---|---|
| `grep 'apple a1'` | `grep`, `apple a1` |
| `echo ab"cd ef"` | `echo`, `abcd ef` |
| `echo ""` | `echo`, empty string |
| `echo "it's"` | `echo`, `it's` |

For example, run `./pipex infile "grep 'apple a1'" "wc -l" outfile`.
The invoking shell removes the outer double quotes; Pipex processes the inner
single quotes. A shared scanner first measures and then copies each argument,
so quote removal and argument boundaries follow the same rules in both passes.

Unclosed quotes return NULL with `EINVAL`; allocation failure returns NULL with
`ENOMEM`, after freeing partial results. The caller reports
`pipex: argument parsing: ...` and the affected child exits 1. Empty/space-only
command strings produce no command and exit 127. Pipeline status still comes
from the final child; this is not Bash's whole-script syntax-error handling.

Backslashes remain literal. Newlines are not argument separators. Expansions,
command substitution, globbing, builtins, and shell operators within command
strings are outside this parser's scope; Pipex itself supplies the pipeline
and redirections through its invocation format.

[↑ Back to top](#top)

<a id="why-use-path-when-path-is-missing"></a>

### Why use `path = ""` when PATH is missing?

`path_value()` returns `NULL` when the environment has no `PATH` variable.
Returning `COMMAND_NOT_FOUND` there would skip searching entirely, even when
the requested executable exists in the current directory.

Instead, `path = ""` feeds one empty entry into `split_path_empty_as_dot()`.
The splitter converts that entry to `.`, so both unset PATH and `PATH=""`
search the current directory. This does not add `/bin` or `/usr/bin` as defaults.
Commands containing `/` still use the direct-path branch before PATH lookup.
The empty string is a borrowed literal: the splitter reads it and allocates
the directory strings separately; the literal itself is neither modified nor freed.

A missing command returns `COMMAND_NOT_FOUND` only after the current-directory
search is exhausted; a denied candidate follows the same permission-error
policy as any other PATH entry.

[↑ Back to top](#top)

<a id="path-allocation-and-ownership"></a>

### PATH allocation and ownership

The directory array is zero-initialized, so a failed entry allocation leaves a
NULL terminator for partial-array cleanup. Candidate construction frees its
intermediate prefix even if the second join fails. If a later allocation fails
after a denied candidate was retained, that candidate is freed and the output
pointer is cleared. The resolver then frees the directory array and sets ENOMEM.
The caller frees its command arguments and exits 1, rather than reporting
command not found.

A candidate is either freed by the helper or transferred to the failure-path
output. Later denied candidates are freed; a terminal error frees the previously
retained path before replacing it. On failure the caller reports and frees the
retained path. Successful execve replaces the process and its address space.

[↑ Back to top](#top)

<a id="evaluation-checks"></a>

## Evaluation: 45 checks and examples

Run the included [45-check tester](tests/eval_45.py) from this directory (not included in Campus submission):

```sh
python3 tests/eval_45.py             # automatic run
python3 tests/eval_45.py --demo      # show commands; Enter advances each check
python3 tests/eval_45.py --verbose   # show commands without pausing
```

It requires Python 3.8+, Bash, Make, a C compiler, `ar`, standard Unix commands,
and Valgrind for the final four checks. Run as an ordinary user for permission
checks. It builds a fresh temporary copy of the current source, preserving your
working build, and prints the location of its JSON report and command/output
logs. Nothing is downloaded. The temporary copy includes unsaved-to-Git changes
that have been saved to disk; it does not check whether they were submitted.

Numbering matches the 45 checks below. Missing Valgrind (or `--no-valgrind`)
produces four **SKIP** results, not passes. Exit codes: 0 = all passed,
1 = failures, 2 = skipped/incomplete checks or invalid invocation, 130 = interrupted.
Norm remains a separate check (`norminette src includes libft`). Timing checks
can be affected by a heavily loaded machine. Memory checks trace executed child
programs too, so inspect the named report if a system utility produces an error.


**45/45 generated by A.I. and local checks passed on 21 September 2026**, covering the mandatory
pipeline, build rules, error cases, and both bonuses.
These are 45 individual checks, including related cases, not 45 separate
features or an official grade. Bonus evaluation still depends on passing the
mandatory part. The parsing limits above remain part of the implementation.

The numbered tables reproduce the coverage of that run. Pipeline comparisons
checked output bytes, exit status, stdout, and whether stderr was present;
they did not require identical diagnostic wording. Build checks have no Bash
pipeline equivalent. Heredoc and long-pipeline checks originally used expected
output; the helpers below also provide Bash comparisons for reproduction.
Norm and parser allocation-failure checks are additional, outside the 45.

<details>
<summary>Open setup and reusable Bash/Pipex comparison commands</summary>

Run in Bash, from the project directory, as an ordinary user: root can bypass
permission-denial fixtures. This creates an isolated scratch directory. Keep
this shell open while using the tables. Python 3 supplies the large fixtures;
Valgrind is needed only for the final four checks.

```bash
make
project=$PWD
P="$project/pipex"
check_dir=$(mktemp -d)
cd "$check_dir"
printf 'apple a1\nbanana\napple a1\n' > input
: > empty
python3 -c 'import sys; sys.stdout.write("abc\n" * 300000)' > large
printf x > unreadable
chmod 000 unreadable
mkdir dir
printf '#!/bin/sh\nexit 0\n' > denied
chmod 644 denied
printf '#!/no/such/interpreter\n' > broken
chmod 755 broken

# compare INPUT COMMAND... runs both versions and compares their output/status.
# Example expansion:
# "$P" input "grep 'apple a1'" "wc -l" actual
# bash --noprofile --norc -c "< input grep 'apple a1' | wc -l > expected"
compare() {
    local infile=$1 script cmd actual_status expected_status failed=0
    shift
    printf 'old data\n' > actual
    printf 'old data\n' > expected
    "$P" "$infile" "$@" actual >actual.stdout 2>actual.stderr
    actual_status=$?
    printf -v script '< %q ' "$infile"
    for cmd in "$@"; do script+="$cmd | "; done
    script=${script% | }
    script+=' > expected'
    bash --noprofile --norc -c "$script" >expected.stdout 2>expected.stderr
    expected_status=$?
    printf 'status: pipex=%s bash=%s\n' "$actual_status" "$expected_status"
    cmp actual expected || failed=1
    cmp actual.stdout expected.stdout || failed=1
    test "$actual_status" -eq "$expected_status" || failed=1
    test -s actual.stderr; local a=$?
    test -s expected.stderr; local b=$?
    test "$a" -eq "$b" || failed=1
    return "$failed"
}

# BODY includes the limiter, or ends at EOF without it.
# Bash reads a real here-document from its script input.
heredoc_compare() {
    printf 'existing\n' > actual
    printf 'existing\n' > expected
    "$P" here_doc END cat 'wc -l' actual < "$1"
    printf 'pipex status: %s\n' "$?"
    { printf 'cat <<END | wc -l >> expected\n'; cat "$1"; } | bash
    printf 'bash status: %s\n' "$?"
    cmp actual expected
}
printf 'a\nb\nEND\n' > body
printf 'a\n' > body_eof
printf 'a\nEND' > body_no_newline
python3 -c 'import sys; sys.stdout.write("x\n" * 100000 + "END\n")' > body_large
```

`compare` returns 0 when all comparisons match, or 1 on a mismatch; use
`echo "$?"` immediately afterward. The heredoc helper displays each status
and compares file contents.
Bash may warn when the heredoc ends at EOF without its limiter. That warning
is not an expected byte-for-byte match with Pipex.

</details>

<a id="evaluation-build-and-arguments"></a>

### Build and argument checks

Run Makefile commands in the project directory (`cd "$project"`), then return
to `"$check_dir"` for pipeline examples. For no-relink checks, compare
`stat -c %y pipex` before and after the repeated build.

| # | Result | Check | Command and expected result |
|---|---|---|---|
| 1 | ✅ | Clean mandatory build | `make fclean; make` creates `pipex`. |
| 2 | ✅ | Mandatory no relink | `make` again leaves executable modification time unchanged. |
| 3 | ✅ | Missing binary | `rm pipex; make` recreates it despite the existing build marker. |
| 4 | ✅ | No arguments | `"$P"` returns a nonzero usage status. |
| 5 | ✅ | One argument | `"$P" dummy` returns a nonzero usage status. |
| 6 | ✅ | Two arguments | `"$P" dummy dummy` returns a nonzero usage status. |
| 7 | ✅ | Three arguments | `"$P" dummy dummy dummy` returns a nonzero usage status. |
| 8 | ✅ | Five arguments | `"$P" dummy dummy dummy dummy dummy` is rejected by the mandatory build. |
| 9 | ✅ | Six arguments | `"$P" dummy dummy dummy dummy dummy dummy` is rejected by the mandatory build. |

[↑ Back to top](#top)

<a id="evaluation-pipeline-comparisons"></a>

### Mandatory pipeline comparisons

Each `compare` invocation runs the Pipex and Bash commands shown by the helper
above; its arguments are the exact input/command combinations checked.

| # | Result | Case | Reproduction | Expected status |
|---|---|---|---|---|
| 10 | ✅ | Subject example | `compare input 'grep a1' 'wc -w'` | 0 |
| 11 | ✅ | Normal pipeline | `compare input cat 'wc -l'` | 0 |
| 12 | ✅ | Empty input | `compare empty cat 'wc -c'` | 0 |
| 13 | ✅ | Large stream | `compare large cat 'wc -c'` | 0 |
| 14 | ✅ | Missing input | `compare missing cat 'wc -c'` | 0; final command reads EOF |
| 15 | ✅ | Unreadable input | `compare unreadable cat 'wc -c'` | 0; input error is reported |
| 16 | ✅ | Missing first command | `compare input missing_987123 'wc -l'` | 0 |
| 17 | ✅ | Missing last command | `compare input cat missing_987123` | 127 |
| 18 | ✅ | Non-executable file | `compare input cat ./denied` | 126 |
| 19 | ✅ | Directory as command | `compare input cat ./dir` | 126 |
| 20 | ✅ | Missing script interpreter | `compare input cat ./broken` | 126 on the tested Bash |
| 21 | ✅ | Output is a directory | Output-error examples below, using `dir`. | 1 |
| 22 | ✅ | Output permission denied | Output-error examples below, using files with mode `000`. | 1 |
| 23 | ✅ | Single quotes | `compare input "grep 'apple a1'" 'wc -l'` | 0 |
| 24 | ✅ | Double quotes | `compare input 'grep "apple a1"' 'wc -l'` | 0 |
| 25 | ✅ | Tab separator | `compare input $'grep\ta1' 'wc -l'` | 0 |
| 26 | ✅ | Early reader exit | `compare input yes 'head -n 1'` | 0; no hang |
| 27 | ✅ | Concurrent children | Time commands below. | 0; about one second |
| 28 | ✅ | Earlier side effect survives output failure | Marker commands below. | Both markers exist |

```bash
# 21: separate invocations; record $? immediately after each.
"$P" input cat 'wc -l' dir; echo "$?"
bash -c '< input cat | wc -l > dir'; echo "$?"

# 22: independent protected output files.
printf old > denied_actual; printf old > denied_expected
chmod 000 denied_actual denied_expected
"$P" input cat 'wc -l' denied_actual; echo "$?"
bash -c '< input cat | wc -l > denied_expected'; echo "$?"

# 27: sequential children would take about two seconds.
time "$P" input 'sleep 1' 'sleep 1' actual
time bash -c '< input sleep 1 | sleep 1 > expected'

# 28: remove stale markers before checking side effects.
rm -f actual_marker expected_marker
"$P" input 'touch actual_marker' cat dir
bash -c '< input touch expected_marker | cat > dir'
test -f actual_marker && test -f expected_marker
```

[↑ Back to top](#top)

<a id="evaluation-bonus-and-cleanup"></a>

### Bonus and cleanup checks

| # | Result | Check | Reproduction and expected result |
|---|---|---|---|
| 29 | ✅ | Bonus build | `make -C "$project" bonus` succeeds. |
| 30 | ✅ | Bonus no relink | Repeat bonus build; executable modification time stays unchanged. |
| 31 | ✅ | Multiple pipes | `compare input cat 'grep a1' 'wc -l'` produces `2` and newline. |
| 32 | ✅ | Normal heredoc | `heredoc_compare body` appends `2` and newline after `existing`. |
| 33 | ✅ | EOF before limiter | `heredoc_compare body_eof` appends `1` and newline. |
| 34 | ✅ | Limiter without newline | `heredoc_compare body_no_newline` appends `1`, excluding `END`. |
| 35 | ✅ | Large heredoc | `heredoc_compare body_large` appends `100000` and newline without blocking. |
| 36 | ✅ | Clean | `make -C "$project" clean` removes objects and retains the executable. |
| 37 | ✅ | Rebuild | `make -C "$project" re` recreates the mandatory executable. |
| 38 | ✅ | Full clean | `make -C "$project" fclean` removes objects, executable and library archive. |
| 39 | ✅ | Five pipes | Build bonus again; run the five-pipe example below. |
| 40 | ✅ | Twenty pipes | Run the twenty-pipe example below. |
| 41 | ✅ | Repeated heredoc append | Three invocations below preserve `original` and append three `1` lines. |

For checks 39–41, the input contains two lines, matching the original checks:

```bash
make -C "$project" bonus
printf 'a\nb\n' > input_two
compare input_two cat cat cat cat cat 'wc -l'
commands=()
for ((i=0; i<20; i++)); do commands+=(cat); done
compare input_two "${commands[@]}" 'wc -l'

printf 'original\n' > actual
printf 'original\n' > expected
for ((i=0; i<3; i++)); do
    printf 'a\nEND\n' | "$P" here_doc END cat 'wc -l' actual
    bash -c $'cat <<END | wc -l >> expected\na\nEND\n'
done
cmp actual expected
```

The normal-heredoc check also supplied text after the limiter (`ignored`),
confirming Pipex excluded it. Do not append arbitrary text after `END` in the
Bash-script helper: Bash would interpret it as another command.

[↑ Back to top](#top)

<a id="evaluation-memory-checks"></a>

### Memory checks

Use `V=(valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes --error-exitcode=99)`
in Bash. Each row checks Pipex under Valgrind; the Bash column is its behavioral
reference, not a Valgrind test of Bash. Check the child reports as well as the
parent. The memory pass condition was zero reported memory errors and no
lost allocations; inherited descriptors must be distinguished from Pipex FDs.

| # | Result | Mode | Pipex command | Bash reference |
|---|---|---|---|---|
| 42 | ✅ | `make` | `"${V[@]}" "$P" input_two cat missing_93811 actual` | `< input_two cat \| missing_93811 > expected` |
| 43 | ✅ | `make` | `"${V[@]}" "$P" missing cat 'wc -l' actual` | `< missing cat \| wc -l > expected` |
| 44 | ✅ | `make` | `"${V[@]}" "$P" input_two missing_93811 missing_93812 actual` | `< input_two missing_93811 \| missing_93812 > expected` |
| 45 | ✅ | `make bonus` | `printf 'a\nEND\n' \| "${V[@]}" "$P" here_doc END missing_93811 missing_93812 actual` | `bash -c $'missing_93811 <<END \| missing_93812 >> expected\na\nEND\n'` |

Separately, the parser passed ten argument cases, three invalid-input checks,
and 27 injected allocation failures, with zero Valgrind errors/leaks. Those
checks cover empty arguments, adjacent fragments and partial-allocation cleanup;
they are not counted again in the 45. Full source Norm checks also passed.

After reviewing the examples, restore fixture permissions if necessary and
return to the project with `cd "$project"`. All generated example files are
inside the directory printed by `printf '%s\n' "$check_dir"`.

[↑ Back to top](#top)

<a id="reference-comparison-darrens-pipex"></a>

## Reference comparison: Darren's Pipex

I used [Darren's Pipex](https://github.com/whatisthisbuffoonery/showcase-repository/tree/eabd10adcf41950fb9eb6c3f69477d6ecb958b3b/pipex)
as a concrete peer reference, not a measure of every student's implementation.
This comparison is a source review of the local checkout at `eabd10a`, not a
runtime benchmark or a judgment of his submitted version.

Both implementations include multiple pipes and heredoc append. In that
snapshot, his [command lookup](https://github.com/whatisthisbuffoonery/showcase-repository/blob/eabd10adcf41950fb9eb6c3f69477d6ecb958b3b/pipex/cmd.c)
stops at the first existing PATH candidate and uses a delimiter-dropping split.
This version additionally retries permission-denied candidates and preserves
empty PATH entries. His implementation creates the pipeline's pipe array in
advance; mine creates the next pipe as it advances, keeping the parent's live
pipe-FD count constant as the command count grows.

Darren's [argument parser](https://github.com/whatisthisbuffoonery/showcase-repository/blob/eabd10adcf41950fb9eb6c3f69477d6ecb958b3b/libft/ft/parsed_argsplit.c)
handles basic quotes and tab separators, as this version now does too. This
parser additionally joins adjacent quoted/unquoted fragments into one argument
(for example, `ab"cd ef"`). His parser also separates on newlines; mine separates
on spaces and tabs. Neither comparison implies full shell-language support.

[↑ Back to top](#top)

<a id="resources"></a>

## Resources

[↑ Back to top](#top)

<a id="references"></a>

### References

- System-call manuals: `man 2 pipe`, `man 2 fork`, `man 2 dup2`,
  `man 2 execve`, `man 2 open`, `man 2 waitpid`, and `man 2 unlink`.
- [GNU Bash: redirections](https://www.gnu.org/software/bash/manual/html_node/Redirections.html)
  for input/output redirection and here-documents.
- [GNU Bash: invocation](https://www.gnu.org/software/bash/manual/html_node/Invoking-Bash.html)
  for shell invocation and input modes.
- [GNU Bash: exit status](https://www.gnu.org/s/bash/manual/html_node/Exit-Status.html)
  for interpreting command results.
- [Zsh: redirection](https://zsh.sourceforge.io/Doc/Release/Redirection.html)
  for the shell comparisons discussed in the technical notes.
- [Bundled libft documentation](libft/README.md), including
  [GNL state and cleanup](libft/ryker_ft/gnl_status/README.md) and
  [temporary-file creation](libft/ryker_ft/file_unique/README.md).

[↑ Back to top](#top)

<a id="use-of-ai"></a>

### Use of AI

AI helped me scaffold the initial project in the style of my FdF repository,
call and edit functions (e.g. manual work/ Norm spaces correction) under my direction, test edge cases, and check the code against the subject and Norm. More importantly, I used it for a learning
discussion: why concurrent commands do not require every pipe to be stored,
why an unread here-document pipe can fill and block, how exact limiter matching
works, and how real shells can choose different internal storage while keeping
the same behavior.

Ideas recorded above include questions and deductions I brought into that
discussion, such as recognising that `>>` implies append mode and asking how
simultaneous processes affect pipe creation. This was not a request to "vibe
code" a submission I cannot explain. AI performed much of the mechanical
editing and function calling, but I have read, understood, tested, and edited every
part before submission. I do not present code I do not understand as my own.

[↑ Back to top](#top)
