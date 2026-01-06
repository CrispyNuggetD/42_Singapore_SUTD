/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_printer.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 02:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/01/06 14:42:26 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"


static int	write_repeat(t_context *context, char c, size_t count);
static int	write_guaranteed(t_context *context, const char *buf, size_t len);
static int	print_until_padding_for_zeros(t_context *context, t_print *paper);
static int	print_minus(t_context *context, t_print *paper);

int send_for_printing(t_context *context, t_print *paper)
{
	if (context -> spec -> flags & FLAG_MINUS)
	{
		if (print_minus(context, paper) < 0)
			return (-1);
	}
	if (paper -> pad_char == '0')
	{
		if (print_until_padding_for_zeros(context, paper) < 0)
			return (-1);
	}
	else if (write_repeat(context, ' ', paper -> pad_len) < 0)
		return (-1);
	if (paper -> pad_char != '0')
	{
		if (paper -> sign && write_guaranteed(context, &paper -> sign, 1) < 0)
			return (-1);
		if (paper -> prefix_len && write_guaranteed(context,
				paper -> prefix, paper -> prefix_len) < 0)
			return (-1);
	}
	if (write_repeat(context, '0', paper -> prec_zeros) < 0)
		return (-1);
	return (write_guaranteed(context, paper -> core, paper -> core_len));
}

static int	print_minus(t_context *context, t_print *paper)
{
	if (paper -> sign && write_guaranteed(context, &paper -> sign, 1) < 0)
		return (-1);
	if (paper -> prefix_len && write_guaranteed(context,
			paper -> prefix, paper -> prefix_len) < 0)
		return (-1);
	if (write_repeat(context, '0', paper -> prec_zeros) < 0)
		return (-1);
	if (write_guaranteed(context, paper -> core, paper -> core_len) < 0)
		return (-1);
	return (write_repeat(context, ' ', paper -> pad_len));
}

static int	print_until_padding_for_zeros(t_context *context, t_print *paper)
{
	if (paper -> sign && write_guaranteed(context, &paper -> sign, 1) < 0)
		return (-1);
	if (paper -> prefix_len && write_guaranteed(context,
			paper -> prefix, paper -> prefix_len) < 0)
		return (-1);
	if (write_repeat(context, '0', paper -> pad_len) < 0)
		return (-1);
	return (0);
}
//Print till padding of zeros

/*

But you still need the emit order part. Here are the three cases you must implement:

Emit order logic

First, minus flag is most exclusive cos entire output shifts if present
If (MINUS flag) pad on right (Special *5):
1)sign 2)prefix 3)preczeros 4)core *5)pad(spaces)

(Special too)
Else if (pad_char == '0') (Special *3):
1)sign 2)prefix *3)pad(zeros) 4)preczeros 5)core

(Normal)
Else (Special *`):
1*)pad(spaces) 2)sign 3)prefix 4)preczeros 5)core

*/

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