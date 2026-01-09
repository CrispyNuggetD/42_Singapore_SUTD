/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_d_i.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 09:34:40 by hnah              #+#    #+#             */
/*   Updated: 2026/01/09 17:57:20 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static size_t		itoa_no_sign(long long n, char buf[21], const char **start);
static void			print_sign(const t_context *context, t_print *paper, \
long long arg);
static long long	read_signed(va_list arg, t_length length);

int	ft_printf_d_i(t_context *context)
{
	char		text[21];
	const char	*start;
	long long	arg;
	size_t		digit_len;
	t_print		paper;

	ft_printf_init_t_print(&paper);
	arg = read_signed(*(context->input), context->spec->length);
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

static long long	read_signed(va_list arg, t_length length)
{
	long long	temp_arg;

	temp_arg = 0;
	if (length == LEN_NONE)
		temp_arg = (long long)va_arg(arg, int);
	else if (length == LEN_HH)
		temp_arg = (long long)(signed char)va_arg(arg, int);
	else if (length == LEN_H)
		temp_arg = (long long)(short)va_arg(arg, int);
	else if (length == LEN_L)
		temp_arg = (long long)va_arg(arg, long);
	else
		temp_arg = va_arg(arg, long long);
	return (temp_arg);
}

static void	print_sign(const t_context *context, t_print *paper, long long arg)
{
	if (arg < 0)
		paper->sign = '-';
	else if (context->spec->flags & FLAG_PLUS)
		paper->sign = '+';
	else if (context->spec->flags & FLAG_SPACE)
		paper->sign = ' ';
}

static size_t	itoa_no_sign(long long n, char buf[21], const char **start)
{
	int					i;
	unsigned long long	n_magnitude;

	i = 20;
	if (n >= 0)
		n_magnitude = (unsigned long long)(n);
	else
		n_magnitude = (unsigned long long)(-(n + 1)) + 1;
	buf[i] = '\0';
	if (n_magnitude == 0)
	{
		buf[--i] = '0';
		*start = &buf[i];
		return (1);
	}
	while (n_magnitude != 0)
	{
		buf[--i] = '0' + (n_magnitude % 10);
		n_magnitude /= 10;
	}
	*start = &buf[i];
	return ((size_t)(&buf[20] - *start));
}
// 4 will be i = 9
// 42 will be i = 8
