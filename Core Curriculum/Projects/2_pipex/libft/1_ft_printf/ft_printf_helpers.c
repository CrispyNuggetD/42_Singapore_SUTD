/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_helpers.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 12:17:39 by hnah              #+#    #+#             */
/*   Updated: 2026/09/14 03:44:33 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

// Close va_list and return error
/* int	ft_printf_error_end_stream(va_list *input)
{
	va_end(*input);
	return (-1);
} */

void	ft_printf_init_t_print(t_print *paper)
{
	ft_memset(paper, 0, sizeof(*paper));
	paper->pad_char = ' ';
}

void	ft_printf_init_t_context(t_context *context)
{
	ft_memset(context, 0, sizeof(*context));
	context->fd = ft_printf_fd_setting(-1);
}

int	write_guaranteed(t_context *context, const char *buf, size_t len)
{
	ssize_t		written;

	while (len > 0)
	{
		written = write(context->fd, buf, len);
		if (written <= 0)
			return (-1);
		context->printed += (size_t)written;
		buf += written;
		len -= (size_t)written;
	}
	return (0);
}
//This is the actual writer. len > 0 settles partial printing.

int	write_repeat(t_context *context, char c, size_t count)
{
	while (count > 0)
	{
		if (write(context->fd, &c, 1) < 0)
			return (-1);
		context->printed += 1;
		count--;
	}
	return (0);
}

int	is_numeric_conv(int conversion)
{
	if ((conversion) == 'd' || (conversion) == 'i' || (conversion) == 'u' || \
(conversion) == 'x' || (conversion) == 'X')
		return (1);
	return (0);
}

// && is_numeric(spec->conversion)
// Needed as precision suppresses 0 padding only for numeric conversions

/*
Clear the destination directly with our own ft_memset. A zero-initialized
compound literal followed by struct assignment can generate external memset
and memcpy calls with the campus compiler. Restore nonzero defaults afterward.
*/
