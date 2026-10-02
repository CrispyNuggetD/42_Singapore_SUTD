# Variadic integer maximum — WIP study copy

AI-assisted draft saved for later manual review. Not integrated into Libft's
build or public headers. Uses a distinct name to preserve the existing two-int
`ryker_ft_max` interface.

```c
int ryker_ft_max_variadic(int count, ...);

maximum = ryker_ft_max_variadic(4, 12, 7, 30, 9); /* 30 */
maximum = ryker_ft_max_variadic(3, -8, -2, -15);  /* -2 */
```

The first argument counts the following int values. Every consumed argument
must have type int after the default argument promotions. Too few arguments or
incompatible types cause undefined behaviour; extra arguments are ignored.
There is no automatic argument count discovery. For count <= 0, this helper
returns 0 by convention and reads no variable arguments.

va_start begins traversal, va_arg reads the next int, and va_end finishes it.
Initialising the maximum from the first value handles all-negative inputs.
The decrement is a separate statement rather than an operation in the while
condition. This is a runtime function, not a preprocessor maximum for constant
array sizes. Check project-specific allowed functions before integrating it.
