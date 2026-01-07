/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_pointer.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 09:34:40 by hnah              #+#    #+#             */
/*   Updated: 2026/01/08 04:17:45 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static void	set_prefix(uintptr_t addr, t_print *paper);
static size_t	itohtoa_no_sign_small(unsigned int n, char buf[12], const char **start);

int	ft_printf_pointer(t_context *context)
{
	char		text[12];
	const char	*start;
	uintptr_t	addr;
	size_t		digit_len;
	t_print		paper;

	ft_printf_init_t_print(&paper);
	addr = (uintptr_t)va_arg(*(context->input), void *);
	digit_len = itohtoa_no_sign_small(addr, text, &start);
	//if ((context->spec->flags & FLAG_PREC) && \
context->spec->precision == 0 && arg == 0)
	if (addr == 0)
	{
		digit_len = 1;
		paper.core = "0";
	}
	if ((context->spec->flags & FLAG_PREC) && context->spec->precision >= 0 \
&& (size_t)context->spec->precision > digit_len)
		paper.prec_zeros = (size_t)context->spec->precision - digit_len;
	paper.core_len = digit_len + paper.prec_zeros;
	paper.core = start;
	set_prefix(addr, &paper);
	return (ft_printf_print_config(context, &paper));
}
//sign is supposed to be handled by ft_print_sign_handler
//If ever support *, negative precision means “precision not specified"
//hence keep precision >= 0

static size_t	itohtoa_no_sign_small(unsigned int n, char buf[12], const char **start)
{
	const char *hex_buf = "0123456789abcdef";
	int	i;
	int	hex_temp;

	i = 10;
	buf[11] = '\0';
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

static void	set_prefix(uintptr_t addr, t_print *paper)
{
	if (addr != 0)
	{
		paper->prefix = "0x";
		paper->prefix_len = 2;
	}
}