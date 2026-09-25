# Integer math helpers

Public declarations are in `../ryker_ft.h`, included by `ryker_libft.h`.

| Function | Contract |
| --- | --- |
| `int ryker_ft_sign(int value)` | Returns `-1` for negative, `0` for zero, `1` for positive. Valid for every `int`, including `INT_MIN`; no side effects. |
| `int ryker_ft_update_max(int *dest, int candidate)` | Replaces `*dest` only when the candidate is larger. |
| `int ryker_ft_update_min(int *dest, int candidate)` | Replaces `*dest` only when the candidate is smaller. |
| `int ryker_ft_max(int one, int two)` | Returns the larger value. All `int` inputs are valid; equal inputs return that value. |
| `int ryker_ft_abs(int one)` | Returns the absolute value. The caller must exclude `INT_MIN`, whose positive magnitude cannot fit in `int`. No runtime guard is performed. |

No helper allocates memory. The update helpers modify only `*dest`; ties leave
it unchanged. Both return `-1` when `dest` is null, `0` when unchanged (including ties),
and `1` when the destination is updated. A non-null `dest` must point
to a valid, initialised `int`.
All integer values, including `INT_MIN` and `INT_MAX`, are valid for updates. The library Makefile
includes these sources automatically through `ryker_ft/*/*.c`.
