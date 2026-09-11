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
