/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_d_i.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 09:34:40 by hnah              #+#    #+#             */
/*   Updated: 2026/01/08 03:41:51 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static size_t	itoa_no_sign(int n, char buf[12], const char **start);
static void		print_sign(const t_context *context, t_print *paper, int arg);

int	ft_printf_d_i(t_context *context)
{
	char		text[12];
	const char	*start;
	int			arg;
	size_t		digit_len;
	t_print		paper;

	ft_printf_init_t_print(&paper);
	arg = va_arg(*(context->input), int);
	print_sign(context, &paper, arg);
	digit_len = itoa_no_sign(arg, text, &start);
	if ((context->spec->flags & FLAG_PREC) && \
context->spec->precision == 0 && arg == 0)
		digit_len = 0;
	if ((context->spec->flags & FLAG_PREC) && context->spec->precision >= 0 \
&& (size_t)context->spec->precision > digit_len)
		paper.prec_zeros = (size_t)context->spec->precision - digit_len;
	paper.core_len = digit_len;
	paper.core = start;
	return (ft_printf_print_config(context, &paper));
}
//sign is supposed to be handled by ft_print_sign_handler
//If ever support *, negative precision means “precision not specified"
//hence keep precision >= 0

static void	print_sign(const t_context *context, t_print *paper, int arg)
{
	if (arg < 0)
		paper->sign = '-';
	else if (context->spec->flags & FLAG_PLUS)
		paper->sign = '+';
	else if (context->spec->flags & FLAG_SPACE)
		paper->sign = ' ';
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
	return ((size_t)(&buf[11] - *start));
}
// 4 will be i = 9
// 42 will be i = 8
