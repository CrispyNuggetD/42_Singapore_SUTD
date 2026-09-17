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
