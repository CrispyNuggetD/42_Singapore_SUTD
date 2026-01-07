/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_helpers.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 12:17:39 by hnah              #+#    #+#             */
/*   Updated: 2026/01/07 12:46:09 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

// Close va_list and return error
int	ft_printf_error_end_stream(va_list *input)
{
	va_end(*input);
	return (-1);
}

void	ft_printf_init_t_print(t_print *paper)
{
	*paper = (t_print){0};
	paper -> pad_char = ' ';
}

void	ft_printf_init_t_context(t_context *context)
{
	*context = (t_context){0};
}

int	write_guaranteed(t_context *context, const char *buf, size_t len)
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

int	write_repeat(t_context *context, char c, size_t count)
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
Compound literal (t_print){0} creates a temporary t_print value.
Because of {0}, the first field is init 0, 
and all the rest become 0 too.

This simplistic init is same as:
static void	init_t_print(t_print *paper)
{
	paper -> sign = 0;
	paper -> prefix = NULL;
	paper -> prefix_len = 0;
	paper -> core = NULL;
	paper -> core_len = 0;
	paper -> prec_zeros = 0;
	paper -> pad_len = 0;
	paper -> pad_char = ' ';
	paper -> left = 0;
}
*/