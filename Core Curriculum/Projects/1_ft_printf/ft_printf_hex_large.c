/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_hex_large.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 09:34:40 by hnah              #+#    #+#             */
/*   Updated: 2026/01/08 17:31:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static void					set_prefix(t_context *context, t_print *paper, \
unsigned long long arg, int small);
static size_t				itohtoa_no_sign_big(unsigned long long n, \
char buf[17], const char **start);
static unsigned long long	read_unsigned(va_list arg, t_length length);


int	ft_printf_hex_big(t_context *context)
{
	char				text[17];
	const char			*start;
	unsigned long long	arg;
	size_t				digit_len;
	t_print				paper;

	ft_printf_init_t_print(&paper);
	arg = va_arg(*(context->input), unsigned int);
	digit_len = itohtoa_no_sign_big(arg, text, &start);
	if ((context->spec->flags & FLAG_PREC) && \
context->spec->precision == 0 && arg == 0)
		digit_len = 0;
	if ((context->spec->flags & FLAG_PREC) && context->spec->precision >= 0 \
&& (size_t)context->spec->precision > digit_len)
		paper.prec_zeros = (size_t)context->spec->precision - digit_len;
	paper.core_len = digit_len;
	paper.core = start;
	set_prefix(context, &paper, arg, 0);
	return (ft_printf_print_config(context, &paper));
}
//sign is supposed to be handled by ft_print_sign_handler

static unsigned long long	read_unsigned(va_list arg, t_length length)
{
	long long	temp_arg;

	if (length == LEN_NONE)
		temp_arg = (unsigned long long)va_arg(arg, unsigned int);
	else if (length == LEN_HH)
		temp_arg = \
(unsigned long long)(unsigned char)va_arg(arg, unsigned int);
	else if (length == LEN_H)
		temp_arg = (unsigned long long)(short)va_arg(arg, unsigned int);
	else if (length == LEN_L)
		temp_arg = (unsigned long long)va_arg(arg, unsigned long);
	else if (length == LEN_LL)
		temp_arg = va_arg(arg, unsigned long long);
	return temp_arg;
}

static size_t	itohtoa_no_sign_big(unsigned long long n, char buf[17], \
const char **start)
{
	const char *hex_buf = "0123456789ABCDEF";
	int			i;
	int			hex_temp;

	i = 10;
	buf[16] = '\0';
	if (n == 0)
	{
		buf[i] = '0';
		*start = &buf[i];
		return (1);
	}
	while (n != 0)
	{
		hex_temp = n % 16;
		buf[i] = hex_buf[hex_temp];
		n /= 16;
		i--;
	}
	*start = &buf[i + 1];
	return ((size_t)(&buf[11] - *start));
}

static void	set_prefix(t_context *context, t_print *paper, \
unsigned long long arg, int small)
{
	if (arg != 0 && (context->spec->flags & FLAG_HASH))
	{
		if (small)
		{
			paper->prefix = "0x";
			paper->prefix_len = 2;
		}
		else
		{
			paper->prefix = "0X";
			paper->prefix_len = 2;
		}
	}
}