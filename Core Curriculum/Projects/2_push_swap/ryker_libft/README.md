*This library combines projects created as part of the 42 curriculum by hnah.*

# Ryker libft

## Description

This is the expanded master package copied as a whole into later projects.
`Projects/0_libft` remains the base project archive.

| Directory | Contents |
| --- | --- |
| `0_libft` | Base libft sources |
| `1_ft_printf` | Original formatter with configurable FD and shared argument-list entry |
| `1_get_next_line` | GNL sources; the package builds the ordinary variant |
| `ryker_ft` | Custom helpers, including the FD printf wrapper and Push_swap's existing space helper |

## Instructions

Run `make` to build `libft.a`, `make clean` to remove objects, or `make fclean`
to remove both objects and the library. The package builds bonus printf.
The master Makefile compiles each component directly into separate object
directories and archives them together, without embedding archives inside archives.

Copy the complete `ryker_libft/` directory into a consuming project. Its Makefile
should invoke `$(MAKE) -C ryker_libft` and link `ryker_libft/libft.a`.

Include the package header from a project header in `includes/`:

```c
#include "../ryker_libft/ryker_libft.h"
```

For a project header at the project root, use `"ryker_libft/ryker_libft.h"`.
The package header includes its component headers through relative paths, so
consumers need no include search paths into package subdirectories. Headers
provide declarations for compilation; `ryker_libft/libft.a` supplies implementations
for linking. The package Makefile manages its own internal compiler include paths.

Rebuild consumers after replacing the package. Keep project-specific additions
outside the package. Do not distribute `obj/` or `libft.a` between platforms.

## Resources and update history

The [printf update documentation](1_ft_printf/README.md#post-submission-update-custom-fd-and-ryker-libft)
explains why Pipex needed this extension, the changes from the submitted printf,
argument-list forwarding, cleanup, and AI assistance.

`ft_printf(...)` defaults to FD 1. `ryker_ft_printf_fd(fd, ...)` temporarily sets
the destination and restores the previous value after success or an error return.
The setting is process-local and intended for sequential use, not concurrent threads.
The original formatting algorithms are retained. `ryker_ft_isspace` intentionally
preserves Push_swap's existing behavior: it recognizes only the space character.
