/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_d_i.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 09:34:40 by hnah              #+#    #+#             */
/*   Updated: 2026/01/04 18:49:57 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

# PROTOTYPES

int	ft_printf_d_i(t_context *context)
{
	char		text[12];
	const char	*start;
	int			arg;
	int			printer_len;
	t_print 	paper;

	ft_printf_init_t_print(&paper);	
	arg = va_arg(*(context -> input), int);
	print_sign_handler(context, &paper, arg);
	paper.core_len = itoa_no_sign(arg, text, &start);
	paper.core = start;
	printer_len = print_printer(context, &paper);
	return (printer_len);
}
//sign is supposed to be handled by ft_print_sign_handler

static void	print_sign_handler(const t_context *context, t_print *paper, int arg)
{
	if (arg < 0)
		paper -> sign = '-';
	else if (context -> spec -> flags & FLAG_PLUS)
		paper -> sign = '+';
	else if (context -> spec -> flags & FLAG_SPACE)
		paper -> sign = ' ';
}

static size_t	itoa_no_sign(int n, char buf[12], const char **start)
{
	int		i;
	long	n_long;

	i = 10;
	n_long = (long)n;
	if (n < 0)
		n_long = -n_long;
	buf[11] = '\0';
	if (n_long == 0)
	{
		buf[i] = '0';
		*start = &buf[i];
		return (1);
	}
	while (n_long != 0)
	{
		buf[i] = '0' + (n_long % 10);
		n_long /= 10;
		i--;
	}
	*start = &buf[i + 1];
	return ((size_t)(10 - i));
}
// 4 will be i = 9
// 42 will be i = 8

