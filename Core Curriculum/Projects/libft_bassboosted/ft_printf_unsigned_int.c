/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_unsigned_int.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 09:34:40 by hnah              #+#    #+#             */
/*   Updated: 2026/01/09 17:56:44 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static unsigned long long	read_unsigned(va_list arg, t_length length);
static size_t				itoa_no_sign(unsigned long long n_magnitude, \
char buf[21], const char **start);

int	ft_printf_unsigned_int(t_context *context)
{
	char				text[21];
	const char			*start;
	unsigned long long	arg;
	size_t				digit_len;
	t_print				paper;

	ft_printf_init_t_print(&paper);
	arg = read_unsigned(*(context->input), context->spec->length);
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

static unsigned long long	read_unsigned(va_list arg, t_length length)
{
	unsigned long long	temp_arg;

	temp_arg = 0;
	if (length == LEN_NONE)
		temp_arg = (unsigned long long)va_arg(arg, unsigned int);
	else if (length == LEN_HH)
		temp_arg = \
(unsigned long long)(unsigned char)va_arg(arg, unsigned int);
	else if (length == LEN_H)
		temp_arg = \
(unsigned long long)(unsigned short)va_arg(arg, unsigned int);
	else if (length == LEN_L)
		temp_arg = (unsigned long long)va_arg(arg, unsigned long);
	else
		temp_arg = va_arg(arg, unsigned long long);
	return (temp_arg);
}

static size_t	itoa_no_sign(unsigned long long n_magnitude, \
char buf[21], const char **start)
{
	int	i;

	i = 20;
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
