/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 13:22:29 by hnah              #+#    #+#             */
/*   Updated: 2026/01/03 13:47:06 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>
# include "libft/libft.h"
# include <limits.h>

# define FLAG_MINUS 1
# define FLAG_ZERO  2
# define FLAG_PREC  4
# define FLAG_HASH  8
# define FLAG_SPACE 16
# define FLAG_PLUS  32

typedef struct s_spec
{
	unsigned char	flags;
	int				width;
	int				precision;
	char			conversion;
}					t_spec;

typedef struct s_context
{
	va_list	*input;
	t_spec	*spec;
}			t_context;

typedef struct  s_print
{
	char		sign;
	const char	*core;
	size_t		core_len;
	const char	*prefix;
	size_t		prefix_len;
	size_t		prec_zeros;
	size_t		pad_len;
	char		pad_char;
}				t_print;

/*
char *core; and size_t core_len;

What it is: the main payload bytes to print, and how many bytes.

Examples:

%d for 123 → core = "123", core_len = 3

%s for "hello" → core = "hello", core_len = 5

%c for 'A' → core points to a 1-byte buffer containing 'A', core_len = 1

%x for 0x2a → core = "2a", core_len = 2

special case: %.0d with value 0 → core_len = 0 (prints nothing)

Important: for signed numbers, core should be the digits without the - (because the - lives in sign).

char *prefix; and size_t prefix_len;

What it is: an optional string that must appear before the core (after sign, typically).

Examples:

%#x with nonzero value → prefix = "0x", prefix_len = 2

%#X with nonzero value → prefix = "0X", prefix_len = 2

%p → prefix almost always "0x"

most cases → prefix = "" or NULL, prefix_len = 0

This is not the sign. It’s things like "0x".

int prec_zeros;

What it is: the number of leading '0' characters inserted to satisfy precision for numeric conversions.

Example: %.5d and value 42

core = "42" (len 2)

prec_zeros = 5 - 2 = 3

output digits become "00042" (but you don’t build that string; printer prints 3 zeros then core)

Applies to: numeric conversions (d/i/u/x/X/p), not %s, not %c.

Special case: precision is specified as 0 and value is 0 → core_len becomes 0, and prec_zeros is 0 (you print nothing for digits).

int pad_len;

What it is: how many padding characters to print to satisfy width after accounting for everything else.

Formally:

content_len = sign_len + prefix_len + prec_zeros + core_len
pad_len = max(0, width - content_len)


Example: width 8, printing -00042 (len 6) → pad_len = 2.

char pad_char;

What it is: what character the width padding uses:

' ' spaces most of the time

'0' only when the 0 flag is active and it’s allowed (not left-aligned, and not suppressed by precision for numerics)

Crucial nuance: this is for width padding, not precision.
Precision zeros are tracked separately by prec_zeros.

int left;

What it is: whether the field is left-aligned (the - flag).

left = 1 → padding goes on the right

left = 0 → padding goes on the left (or zeros-in-the-middle case)

If you prefer, you can drop this and just read it from spec->flags, but keeping it makes the printer’s life easier.
*/

typedef int	(*t_handler)(t_context *context);

int		ft_printf(const char *key, ...);
int		ft_printf_parse_specs(t_spec *spec, const char **ptr);
int		ft_printf_error_end_stream(va_list *input);
int		ft_printf_d_i(t_context *context);
int		ft_printf_percent(t_context *context);
int		ft_print_printer(t_context *context, const t_print *paper);
void	init_t_print(t_print *paper);

#endif