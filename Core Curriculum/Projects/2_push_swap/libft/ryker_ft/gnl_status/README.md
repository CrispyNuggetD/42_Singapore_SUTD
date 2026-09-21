# GNL with caller-owned state and explicit results

This extension reuses the original GNL helpers while letting a caller stop before
EOF, release buffered data, and distinguish EOF from failure. The original
`char *get_next_line(int fd)` remains available with its subject-compatible API.
It still owns a private static stash; this extension owns no static stash.

## Public API

Include `ryker_libft.h`, or this component's `ryker_ft_get_next_line.h`.

```c
void            ryker_ft_gnl_init(t_gnl_info *gnl, int fd);
t_gnl_result    ryker_ft_get_next_line(t_gnl_info *gnl, char **line);
void            ryker_ft_gnl_cleanup(t_gnl_info *gnl);
```

| Result | Value | Output |
| --- | --- | --- |
| `GNL_LINE` | `1` | Allocated line, including its newline if present |
| `GNL_EOF` | `0` | `NULL`: no more buffered or input text |
| `GNL_ERROR` | `-1` | `NULL`: read, allocation, or argument failure |

Compare the result with the named constants. `if (result)` alone is wrong:
both a line and an error are nonzero.

`t_gnl_info` contains `stash`, `fd`, and `eof`. Initialize a fresh container
before its first call. Both pointer arguments must be valid, and `line` must
point to a separate caller-owned `char *` variable, not the stash member.
Free each returned line before overwriting that output pointer on another call.

`fd` is passed by value because initialization copies its number without changing
the caller's variable. This borrows access to the same open descriptor; it does
not duplicate or take ownership of it. Cleanup never closes the descriptor.
Do not close/reuse the FD or change it while the container has buffered data.
Independent containers can track different FDs; do not mix this API and the
original GNL on the same stream, since each can buffer ahead independently.

Cleanup releases the stash, sets it to `NULL`, and marks the container finished.
It is safe to repeat after initialization. Clear an active container before
reinitializing it, otherwise its stash pointer would be overwritten and leaked.
Reinitialization starts at the FD's current position; it does not rewind input.
After a read/allocation error, cleanup is automatic. Initialize again if retrying;
a failed call may already have consumed input, so it cannot promise to recover it.

## Example: stopping at a limiter

```c
t_gnl_info      gnl;
t_gnl_result    result;
char            *line;

ryker_ft_gnl_init(&gnl, STDIN_FILENO);
result = ryker_ft_get_next_line(&gnl, &line);
while (result == GNL_LINE && !is_limiter(line, limiter))
{
    if (write_line_guaranteed(fd, line) < 0)
    {
        free(line);
        ryker_ft_gnl_cleanup(&gnl);
        return (-1);
    }
    free(line);
    result = ryker_ft_get_next_line(&gnl, &line);
}
free(line);
ryker_ft_gnl_cleanup(&gnl);
/* GNL_ERROR aborts setup; GNL_EOF is ordinary end-of-input. */
```

This illustrates Pipex's `collect_heredoc_lines()`; its `heredoc_cleanup()` helper
combines freeing the line, clearing GNL state, and returning a supplied status. Cleanup occurs on delimiter,
EOF, read failure, allocation failure, and output failure. It frees the returned
line separately from the stash. A final line without newline still returns
`GNL_LINE`; only the next call reports `GNL_EOF`.

## Implementation and limits

- `read_chunk()` reads `BUFFER_SIZE` bytes and uses the original `gnl_strjoin()`.
  Its internal integer result indicates chunk success/failure, not line/EOF state.
- `find_len(stash, '\n')` locates the buffered prefix. `nl` is the newline's
  one-based position when found, otherwise the current string length. The final
  character check distinguishes these cases. `break` stops collecting bytes and
  sends both newline and EOF paths to `take_line()`.
- `take_line()` uses the original `newline_ret()`, converts its result to the enum,
  and frees remaining state on allocation failure. EOF with an empty stash is
  handled separately from allocation failure.
- The extension has a separate control loop, not a call around the original
  `get_next_line()`: an outside wrapper cannot access that function's private stash
  or determine why it returned `NULL`. Splitting and joining are shared unchanged.
- No fixed FD array is used, so no arbitrary 1024 cap is needed. The original bonus
  GNL still has its own documented fixed-array capacity.
- Buffered reads may consume bytes after a limiter. Cleanup discards those bytes;
  it cannot return them to the stream. That is suitable for this Pipex heredoc
  path, but a future interactive shell must retain relevant input or control
  read-ahead when it will subsequently parse commands from the same stream.
- This remains a text-oriented GNL. It does not add binary input support, retry
  interrupted reads, or improve the original concatenation algorithm's cost for
  very long lines. It is not a complete allocation-overflow audit.

The component lives in main Ryker libft and is copied into Pipex, FdF, and
push_swap. The library Makefile discovers its C file automatically, and
`ryker_ft.h` exposes its header. Original GNL submission files are not rewritten
into this interface. Existing allocation-failure behavior in that original API
is separate; this extension cleans up its own state on those helper failures.

## Validation (2026-09-18)

The API passed ASan/UBSan checks with `BUFFER_SIZE` 1, 67, and 4096, including
final lines without newline, repeated EOF, early/repeated cleanup, keeping the
borrowed FD open, read errors, and injected allocation failures with tracked
allocation balance. Pipex mandatory/bonus builds and normal pipeline checks
passed; heredoc checks covered append, exact and prefix delimiters, trailing
input after the limiter, EOF without a newline, empty input, large input, and
closed stdin. All four library copies built, and changed C/header files passed
Norminette. This is focused validation, not a full fault-injection audit of Pipex.
