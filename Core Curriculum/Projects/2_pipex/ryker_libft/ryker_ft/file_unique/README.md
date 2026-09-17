# Create and open unique files

This component adds numeric suffixes to a caller-supplied prefix and creates a
new ordinary file without overwriting an existing pathname. It is not limited
to `/tmp/`; whether the file is temporary is the caller's decision.

## API

```c
int ryker_ft_create_open_unique_file(char **new_filepath,
    const char *prefix_filepath);
```

The declarations are in `ryker_ft_file_unique.h`, exposed by `ryker_ft.h` and
`ryker_libft.h`. The public function returns an open write-only FD on success, or
`-1` on failure. Check `fd < 0`: descriptor zero can be a valid success.

| Property | Contract |
| --- | --- |
| Output path | Allocated string on success; `NULL` on failure when the output pointer is valid |
| Output argument | Valid pointer to a separate `char *`; any previous allocation remains the caller's responsibility. Do not alias it with input strings. |
| Ownership | Caller closes the FD, frees the pathname string, and chooses whether/when to unlink the file |
| File creation | `O_WRONLY \| O_CREAT \| O_EXCL`, requested mode `0600` filtered by umask |
| Collision policy | Try suffixes `0` through `INT_MAX - 1`; only `EEXIST` triggers another candidate |
| Errors | Invalid/empty arguments: `EINVAL`; allocation failure: `ENOMEM`; other failures preserve the relevant system `errno`; exhausted suffix range: `EEXIST` |
| Failure cleanup | Frees candidate strings; never unlinks a pathname whose creation failed |

The signed `int` counter uses `number < INT_MAX`. After trying `INT_MAX - 1`,
it increments safely to `INT_MAX` and the loop stops. No cast or overflowing
increment is needed; the final possible suffix is deliberately not attempted.
Successful calls do not promise to clear `errno`; examine it only after failure.
No helper prints diagnostics or exits the process.

## Existing-directory variant

```c
fd = ryker_ft_create_open_unique_file(&path, "/tmp/pipex_here_doc_");
/* e.g. path == "/tmp/pipex_here_doc_0" */
```

The parent directory must exist. Relative prefixes such as `"./output_"` also
work. This variant never calls `mkdir()` or `stat()` and does not remove files.
`append_numeric_suffix()` is private: generating a string does not establish
uniqueness; successful exclusive creation does.

## Directory-creation wrapper — WIP, not public

**Status: WIP.** This experimental implementation is retained for study in main
Ryker libft, in the same component directory. I have not yet reviewed and
understood it sufficiently to take ownership of it. Previous passing tests are
not a declaration that it is complete, approved, or suitable for submission.

It has no public prototype and is explicitly excluded by `WIP_SRCS` in the
Makefile. Its `.c` file is entirely omitted from Pipex's embedded library.
Do not reintroduce it when syncing the master package to Pipex. The example
below documents the proposed interface, not a currently available library API.

```c
fd = ryker_ft_create_open_unique_file_in_dir(&path, "./scratch", "output_");
/* Creates ./scratch if missing, then ./scratch/output_0 (or next free suffix). */
```

The wrapper creates **one directory level**, requesting `0700` subject to umask.
It accepts an existing directory without changing its permissions and rejects
an existing non-directory with `ENOTDIR`. A trailing slash is accepted. The
filename prefix must be nonempty and contain no `/`; the directory is supplied
separately. Missing parent directories are an error, not recursively created.
An existing symlink to a directory is followed. Use trusted directory paths;
these helpers do not provide confinement against directory replacement races.

If subsequent file creation fails, a newly created directory is left in place.
The wrapper deliberately does not roll it back: another process may already be
using it. The caller can decide whether it should be removed. Allocation of the
combined prefix happens before directory creation.

## Pipex and allowed functions

The approved file helper and WIP directory wrapper are separate `.c` files.
The WIP wrapper is excluded from every normal library build, not merely unused
at link time. Pipex's copy contains neither that source nor its declaration;
its library and executable must not contain the wrapper or import `mkdir()` or
`stat()`. Commenting out a prototype alone would not prevent compilation.

Pipex uses the returned pathname to reopen its completed heredoc for reading,
then unlinks it. Open descriptors remain usable after unlink. File creation
failure returns a NULL pathname, so cleanup cannot accidentally unlink a
candidate the process never owned. Reopen errors are reported before cleanup
can change `errno`.

## Limits and validation

The suffixes are predictable, not random. Existing entries are protected by
`O_EXCL`, but collision retries can be slow in a crowded or adversarial directory.
A successfully created pathname is not guaranteed to remain unchanged if other
processes can alter that directory. In particular, close/reopen-by-path is not
an atomic secure-temporary-file handoff. Use these helpers in trusted contexts.

Focused ASan/UBSan checks cover requested modes, collisions (including a dangling
symlink), concurrent creation, preservation of existing file contents, missing
parents, invalid inputs, and accepting/rejecting existing directory paths.
The Pipex binary is checked for absent `mkdir`/`stat` imports. Those wrapper
tests describe earlier experimental validation only. The wrapper
is now excluded from builds; Pipex uses only the existing-directory helper.
