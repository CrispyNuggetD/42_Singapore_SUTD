/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_printer.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 02:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/01/04 18:51:21 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	write_repeat(t_context *context, char c, size_t count);
static int	write_guaranteed(t_context *context, const char *buf, size_t len);
static int	send_for_printing(t_context *context, t_print *paper);

int	ft_printf_set_print_format(t_context *context, t_print *paper)
{
	size_t	content_len;
	int		sign_len;

	sign_len = (paper -> sign != 0);
	content_len = (sign_len + paper -> prefix_len + paper -> prec_zeros + \
					paper -> core_len);
	if ((context -> spec -> flags & FLAG_PREC) && \
		((context -> spec -> precision) > (paper -> core_len)))
		paper -> prec_zeros = (context -> spec -> precision) - \
								(paper -> core_len);
	if ((context -> spec -> width) > content_len)
		paper -> pad_len = (size_t)((context -> spec -> width) - content_len);
	if (context -> spec -> flags & FLAG_ZERO)
		paper -> pad_char = '0';
	return(ft_printf_print(context, paper));
}

static int send_for_printing(t_context *context, t_print *paper)
{

	return (0);
}

/*


But you still need the emit order part. Here are the three cases you must implement:

Emit order logic

If - flag:
sign prefix preczeros core pad(spaces)

Else if pad_char == '0':
sign prefix pad(zeros) preczeros core

Else:
pad(spaces) sign prefix preczeros core

You can implement it literally with your helpers:


*/



/*
figure what the heck is:
if (context -> spec -> flags & FLAG_MINUS)
	{
		if (paper -> sign && ft_write_exact(context, &paper -> sign, 1) < 0)
			return (-1);
		if (paper -> prefix_len && ft_write_exact(context,
				paper -> prefix, paper -> prefix_len) < 0)
			return (-1);
		if (ft_write_repeat(context, '0', paper -> prec_zeros) < 0)
			return (-1);
		if (ft_write_exact(context, paper -> core, paper -> core_len) < 0)
			return (-1);
		return (ft_write_repeat(context, ' ', paper -> pad_len));
	}
	if (paper -> pad_char == '0')
	{
		if (paper -> sign && ft_write_exact(context, &paper -> sign, 1) < 0)
			return (-1);
		if (paper -> prefix_len && ft_write_exact(context,
				paper -> prefix, paper -> prefix_len) < 0)
			return (-1);
		if (ft_write_repeat(context, '0', paper -> pad_len) < 0)
			return (-1);
	}
	else if (ft_write_repeat(context, ' ', paper -> pad_len) < 0)
		return (-1);
	if (paper -> pad_char != '0')
	{
		if (paper -> sign && ft_write_exact(context, &paper -> sign, 1) < 0)
			return (-1);
		if (paper -> prefix_len && ft_write_exact(context,
				paper -> prefix, paper -> prefix_len) < 0)
			return (-1);
	}
	if (ft_write_repeat(context, '0', paper -> prec_zeros) < 0)
		return (-1);
	return (ft_write_exact(context, paper -> core, paper -> core_len));
*/


/*
If Norm’s 25-line rule is biting, that’s exactly why you split into:

ft_write_exact

ft_write_repeat

ft_emit_field

ft_print_printer (compute-only + call emit)

One more architectural sanity fix

Right now main_coordinator ignores dispatch_success except error, and you’re updating context.printed directly during output. That’s totally fine if you decide:

handlers/printer return 0 on success, -1 on error

printed count comes only from context.printed

Then dispatch_key doesn’t need to be called len_printed at all; it’s just a status.

What you should do next (very concrete)

Implement ft_write_exact + ft_write_repeat exactly as above.

Implement ft_emit_field with the 3 ordering cases.

Make ft_print_printer compute prec_zeros/pad_len/pad_char before content_len.

Update %d/%i handler to:

set paper.sign

set paper.core/core_len (your stack itoa_no_sign)

set paper.prefix_len = 0

call ft_print_printer(context, &paper)

Once %d prints correctly with width/zero/left-align, %u is basically copy-paste with no sign logic, and %x/%X adds prefix when #.

If you paste your current %d/%i handler + the is_numeric_conv() function signature/location, I’ll show you exactly how to keep every function under Norm line limits without turning it into a 17-file hydra.*/

static int	write_guaranteed(t_context *context, const char *buf, size_t len)
{
	ssize_t		written;

	while (len > 0)
	{
		written = write(1, buf, len);
		if (written <= 0)
			return (-1);
		context -> printed += (size_t)written;
		buf += written;
		len -= (size_t)written;
	}
	return (0);
}
//This is the actual writer. len > 0 settles partial printing.

static int	write_repeat(t_context *context, char c, size_t count)
{
	while (count > 0)
	{
		if (write(1, &c, 1) < 0)
			return (-1);
		context -> printed += 1;
		count--;
	}
	return (0);
}

/*
Ignored-when-irrelevant:
+ / space only make sense for signed conversions (d/i). They’re ignored for u, x, X, p.
# matters for x/X (and some others in full libc printf), but is ignored for plain decimal d/i.
Precision for %c is typically ignored; precision for %s limits max chars printed; precision for integers is “minimum digits”.
*/

/*
3) Decide ownership (important!)

t_print should not own memory. It’s just “here’s what to print”.
So: if core comes from malloc (ft_itoa), the handler frees it after ft_print_printer() returns.
*/