*This library combines projects created as part of the 42 curriculum by hnah.*

# Ryker libft

An expanded library of completed 42 projects and reusable custom extensions.
`Projects/ryker_libft` is the master package copied into consuming projects;
`Projects/0_libft` remains the base project archive.

## Component index

### Core projects

| Directory | Package behavior | Documentation |
| --- | --- | --- |
| `0_libft/` | Base string, memory, list, and conversion functions | [Libft](0_libft/README.md) |
| `1_ft_printf/` | Builds bonus formatting with configurable output FD support | [Printf](1_ft_printf/README.md) |
| `1_get_next_line/` | Builds ordinary GNL; original bonus sources are also retained | [Get Next Line](1_get_next_line/README.md) |

### Custom extensions

Reusable additions live under `ryker_ft/`. Directory names describe the
customization; related functions share one component directory.

| Component | Public API | Purpose and contract |
| --- | --- | --- |
| [`printf_fd/`](1_ft_printf/README.md#post-submission-update-custom-fd-and-ryker-libft) | `ryker_ft_printf_fd` | Format output to a chosen FD; restores the previous destination afterward. Uses shared process-local state, so it is intended for sequential use, not concurrent threads. |
| [`gnl_status/`](ryker_ft/gnl_status/README.md) | `ryker_ft_gnl_init`, `ryker_ft_get_next_line`, `ryker_ft_gnl_cleanup` | Caller-owned `t_gnl_info`; returns line / EOF / error separately. Cleanup frees buffered data without closing the borrowed FD. |
| [`file_unique/`](ryker_ft/file_unique/README.md) | `ryker_ft_create_open_unique_file` | Creates and opens a file without overwriting existing names. Caller owns FD and allocated path. Directory creation is WIP, excluded from builds and omitted from Pipex. |

### Standalone helpers

Small independent helpers live directly in `ryker_ft/`.

| Function | Contract |
| --- | --- |
| [`ryker_ft_isspace`](ryker_ft/ryker_ft_isspace.c) | Recognizes only `' '`, preserving push_swap's original behavior; it is not a full whitespace classifier. |
| [`ryker_ft_free_str_array`](ryker_ft/ryker_ft_free_str_array.c) | Frees each string in a NULL-terminated allocated array, then the array itself. Accepts `NULL`; does not reset the caller's pointer. |

## Build and integration

| Command | Action |
| --- | --- |
| `make` | Build `libft.a` |
| `make clean` | Remove object files |
| `make fclean` | Remove objects and the library |
| `make re` | Rebuild from scratch |

The Makefile compiles the components into separate object directories and
archives their objects together. It does not embed archives inside archives.

Copy the usable `ryker_libft/` sources into a consuming project, excluding WIP.
In particular, omit `ryker_ft/file_unique/ryker_ft_create_open_unique_file_in_dir.c`
from Pipex. Its Makefile
should invoke `$(MAKE) -C ryker_libft` and link `ryker_libft/libft.a`.
From a project header inside `includes/`, include:

```c
#include "../ryker_libft/ryker_libft.h"
```

For a header at the project root, use `"ryker_libft/ryker_libft.h"`.
The umbrella header includes component headers through relative paths, so
consumers need no additional include paths into library subdirectories.
Headers supply declarations; `libft.a` supplies implementations for linking.

Rebuild consumers after updating their library copies. Do not copy `obj/` or
`libft.a` between platforms. Keep project-specific code outside this package.

## Adding a custom component

1. Develop the component in the master `Projects/ryker_libft` package.
2. Put related sources and their public header in `ryker_ft/<customization>/`.
   Keep a small independent helper directly in `ryker_ft/` when appropriate.
3. Expose the public header or helper declaration through `ryker_ft/ryker_ft.h`.
   That header is already included by `ryker_libft.h`.
4. Add a row to the relevant index above. For a component, add a local README
   describing its API, ownership, errors, limitations, examples, and validation.
5. Record a dated entry below, verify the build and behavior, then sync the
   relevant sources, headers, and documentation into consuming projects.

The current Makefile discovers C files directly in `ryker_ft/` and one directory
below it. Deeper layouts require a source-list change. Experimental work belongs
under `ryker_ft/wip/`, which the build excludes. Colocated experimental sources must also be listed
in `WIP_SRCS`; the directory-creation wrapper is one such exception. Do not expose
WIP in public headers or copy it into Pipex. List it as public only after review.

## Resources and update history

### Detailed references

| Topic | Reference |
| --- | --- |
| Original algorithms, subjects, and learning notes | Core-project READMEs linked in the component index |
| Printf FD configuration, argument forwarding, and shared-state limitations | [Printf extension notes](1_ft_printf/README.md#post-submission-update-custom-fd-and-ryker-libft) |
| OS descriptor limits, the bonus array boundary, and sanitizer troubleshooting | [GNL post-submission update](1_get_next_line/README.md#post-submission-update-file-descriptor-limits) |
| GNL state ownership, explicit results, early cleanup, and Pipex integration | [GNL status extension](ryker_ft/gnl_status/README.md) |

### Updates

Keep entries newest first. Summarize the change here and put implementation
details and test evidence in the linked component documentation.

| Date | Component | Change |
| --- | --- | --- |
| 2026-09-18 | `file_unique` | Extracted exclusive file creation from Pipex, fixed failure cleanup and counter overflow, and retained a separate directory wrapper as WIP (not built or included in Pipex). See [contract and tests](ryker_ft/file_unique/README.md). |
| 2026-09-18 | `gnl_status` | Added caller-owned GNL state, line/EOF/error results, and cleanup; integrated it into Pipex heredoc. See [API and validation](ryker_ft/gnl_status/README.md). |
| 2026-09-18 | Original GNL | Removed the mandatory FD cap and fixed the bonus array guard. Documented FD-limit research and successful ASan/UBSan checks using LLVM. See [bounds update](1_get_next_line/README.md#post-submission-update-file-descriptor-limits). |
| 2026-09-17 | String-array cleanup | Added `ryker_ft_free_str_array` for shared cleanup in Pipex. |
| 2026-09-14 | `printf_fd` | Added configurable output for Pipex while retaining the formatter's algorithms and standard-output default. See [printf update](1_ft_printf/README.md#post-submission-update-custom-fd-and-ryker-libft). |

AI assistance with implementation, documentation, and validation is described in
the core-project and component notes. Each extension's limitations remain part
of its documented contract.
