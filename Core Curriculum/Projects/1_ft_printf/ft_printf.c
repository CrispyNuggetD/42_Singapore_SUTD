/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 13:19:21 by hnah              #+#    #+#             */
/*   Updated: 2026/01/06 13:45:34 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include <stdio.h>
// WARNING DO NOT REMOVE UNTIL STDIO GETS REMOVED: 
// CHANGE MAKEFILE WARNING FLAGS BACK
// NORMINETTE
// HIDDEN FILES
// SUBMISSION CONTENTS
// CHECK INSTANT FAILS OF EVALS

// Let main stay on top (readability) with prototypes
static t_handler	init_get_handlers(unsigned char fn_keys);
static int			dispatch_key(t_context *context);
static int			main_coordinator(const char **str, t_context *context,\
									va_list *input);

// Main function
int	main(void)
{
	int	len;
	int	len2;
	char *input = "tes %% te %i tes";

	len = ft_printf(input, 12);
	ft_printf("\nlen is: %i\n\n", len);
	len2 = printf(input, 12);
	ft_printf("\nlen2 is: %i\n\n", len2);

	//len = ft_printf("ttt%bca%q", 12);
	//ft_printf("\nlen is: %i\n\n", len);
	
}
//len2 = printf("%*s", 10, "asd");
//printf("\nlen2 is: %i\n\n", len2);
//printf("Test ft_itoa(31)\nResult is: %s", ft_itoa(31));

/* 
%[-][0][width][.prec][type]

#### (Class A) 5 Flags

	1. ```0``` flag (zero pad)
		If padding is needed to reach the minimum field width, 
		pad with '0' characters instead of spaces.
	2. ```-``` left align
	3. ```+``` always show sign for signed conversions
	4. space (``` ```) leading space for positive signed conversions
	5. ```#``` alternate form (prefix for hex, etc.)

#### (Class B) 2 Precision features (via ```.```)

	6. ```.``` + digits (**OR only just** ```.```) specifies precision
	7. Precision is “present or not”, and if present it has a number
		- (Number **CAN BE** 0).
*/

// This function is summoned from main to parse the input
int	ft_printf(const char *str, ...)
{
	//int			printed;
	int			is_success;
	t_context	context;
	va_list		input;

	context.printed = 0;
	if (!str)
		return (0);
	va_start(input, str);
	while (*str)
	{
		if (*str != '%')
		{
			ft_putchar_fd(*str, 1);
			str++;
			context.printed += 1;
		}
		else
		{
			is_success = main_coordinator(&str, &context, &input);
			if (is_success < 0)
				return (is_success);
		}
	}
	va_end(input);
	return (context.printed);
}

static int	main_coordinator(const char **str, t_context *context, va_list *input)
{
	int			dispatch_success;
	t_spec		spec;

	(*str)++;
	if (!ft_printf_parse_specs(&spec, str))
		return (ft_printf_error_end_stream(input));
	context -> input = input;
	context -> spec = &spec;
	dispatch_success = dispatch_key(context);
	if (dispatch_success < 0)
		return (ft_printf_error_end_stream(input));
	return (0);
}
// check actual error code if (!str); return (0); 
// The below function initialises (by mapping) the function handlers
// Variables mean type_handler, global_handler

static int	dispatch_key(t_context *context)
{
	t_handler	fn;
	int			len_printed;

	fn = init_get_handlers(context -> spec -> conversion);
	if (fn)
		len_printed = fn(context);
	else
		return (-1);
	return (len_printed);
}

// Initialise keys and also allows dispatcher to fetch those keys
static t_handler	init_get_handlers(unsigned char fn_key)
{
	static t_handler const	handlers[256] = {
	['i'] = ft_printf_d_i,
	['d'] = ft_printf_d_i,
	['%'] = ft_printf_percent,
	};
	return (handlers[fn_key]);
}


/* 


EDGE CASES

ALWAYS CHECK MEMORY LEAKS AND MALLOC AND FREEING

"%*s", 2, *str_ptr

1) Unknown conversion specifier

%q, %y, %k, %!, %= …

% followed by end-of-string: "abc%"

Common libc behaviors vary wildly: some print % then stop, some return -1, some print nothing.
Your ft_printf: pick one rule and stick to it. The most defensible is: return -1, print nothing further, stop.

2) % + garbage after valid flags/width/precision/length

Examples:

"%-0+# 12.5llq" (valid-looking prefix, invalid final)

"%10.zd" (precision dot with non-digit, then conversion)

"%#..3x" (multiple dots)

These are fun because your parser can accidentally:

mis-advance pointer

get stuck (infinite loop)

“accept” nonsense and then dispatch wrong

3) Duplicate or contradictory flags (valid in libc, but easy to mishandle)

Even if you treat them as “valid”, they have precedence rules:

%-0d → 0 ignored because - wins

%+ d → space ignored because + wins

%.0d with value 0 → prints nothing (except width padding)

These aren’t UB, but they produce “weird” outcomes that look like bugs.

4) Width/precision overflow

"%999999999999999999d" (width integer overflow)

"%.999999999999999999d" (precision overflow)

"%*d" with width = INT_MIN / huge negative

"%.*s" with precision huge

If you store width/precision in int, parsing can overflow and go negative or wrap. Pedants love this because it creates memory explosions.

Safe policy: clamp at something sane (e.g., INT_MAX), or detect overflow and error out.

5) * width/precision with missing arguments (va_list misuse)

Format says "%*d" but caller didn’t pass the width arg. That’s UB at the call site; you can’t fully defend. But your code must not make it worse by:

reading extra args due to parse bugs

reading wrong type due to length bugs

6) Wrong type vs length modifier (classic UB)

Examples:

"%ld" but passed int

"%d" but passed long

"%p" but passed non-pointer

"%s" but passed int (or bad pointer)

This is UB by the caller. Your job is: if your parser mis-parses length, you’ll pull the wrong type even when the caller is correct → that’s on you.

7) Length modifier weirdness and invalid combos

Standard-ish modifiers: hh, h, l, ll, j, z, t, L (float).
Edge cases:

"%lll d" (triple l)

"%hhd" (fine) vs "%hhhhhd" (nonsense)

"%lz" (nonsense)

"%I64d" (MSVC-specific; invalid elsewhere)

"%qd" (historical/extension in some worlds)

Your ft_printf likely supports only a subset (42 usually: cspdiuxX% without length). If your parser sees length modifiers but you don’t support them, define what you do:

treat as invalid spec and return -1, or

ignore unsupported length carefully (but don’t desync pointer)

8) Precision with strings and null pointers

"%.s" (precision 0 for string)

"%.0s"

"%.*s" with precision negative (standard: negative precision is “as if precision not specified”)

"%s" with NULL pointer (common: prints (null); but 42 testers vary)

Even if not UB, this is a frequent “gotcha”.

9) NUL characters in output

"%c" with '\0'
This is valid. Output length should still count it. Many student ft_printf fail because they rely on string functions.

10) INT_MIN and negation overflow

"%d" with INT_MIN
If you do n = -n in int, overflow. Must handle specially (or use long).

11) Hex/unsigned conversions and sign mishandling

"%x" with -1 passed as int but read as unsigned: behavior depends on how you cast.
In standard printf, "%x" expects unsigned int; passing int is UB. But testers sometimes still do it. If you want to be robust, cast via unsigned int after reading as unsigned int.

12) Pointer formatting oddities

"%p" for NULL pointer: common output (nil) or 0x0 depending on libc.
42 usually expects 0x0-style (depends on subject/tester).
The pedant trap: ensure prefix 0x, and count length right.

13) Zero precision with zero value

For integers:

"%.0d" with 0 prints nothing (but width still pads)

"%5.0d" with 0 prints 5 spaces
For hex:

"%.0x" with 0 prints nothing (but # flag rules get tricky)

Students often accidentally print "0".

14) % sequences that look like escapes

"%%%d" → prints % then number

"%%%%" → prints %%
Pointer-advance bugs show up immediately here.

15) Embedded invalid UTF-8 / non-ASCII in format

Not UB per se, but if you treat char as signed and index handler table with negative values, you’ll crash:

format contains byte 0xFF
If you do handlers[(char)c] and char is signed ⇒ negative index.

Fix: always cast to unsigned char for table indexing.

16) Reentrancy / state leakage in your t_spec

If you reuse a t_spec struct across conversions and forget to reset fields, you get “phantom flags”.
This shows up in stacked conversions like %i%d%i (your earlier bug smelled like pointer advancement, but spec reset issues can compound it).

17) Format pointer advancement on error

If parse fails, you must ensure you:

either stop entirely (return -1)

or consume at least something so you don’t re-fail forever

Infinite loop is the #1 pedant-killer.

18) va_list lifecycle errors

returning early without va_end

calling va_end twice

passing va_list* around and accidentally copying incorrectly (platform-dependent)

In your code, early returns need a va_end.
*/