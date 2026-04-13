/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_pointer.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 09:34:40 by hnah              #+#    #+#             */
/*   Updated: 2026/01/08 23:30:54 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static void		set_prefix(t_print *paper, uintptr_t arg);
static size_t	itohtoa_no_sign_small(uintptr_t n, char buf[20], \
const char **start);

int	ft_printf_pointer(t_context *context)
{
	char		text[20];
	const char	*start;
	uintptr_t	arg;
	size_t		digit_len;
	t_print		paper;

	ft_printf_init_t_print(&paper);
	arg = (uintptr_t)va_arg(*(context->input), void *);
	if (!arg)
	{
		digit_len = 5;
		start = "(nil)";
	}
	else
		digit_len = itohtoa_no_sign_small(arg, text, &start);
	paper.core_len = digit_len;
	paper.core = start;
	set_prefix(&paper, arg);
	return (ft_printf_print_config(context, &paper));
}
//sign is supposed to be handled by ft_print_sign_handler
//If ever support *, negative precision means “precision not specified"
//hence keep precision >= 0

static size_t	itohtoa_no_sign_small(uintptr_t n, char buf[20], \
const char **start)
{
	const char	*hex_buf = "0123456789abcdef";
	int			i;
	int			hex_temp;

	i = 19;
	buf[i] = '\0';
	if (n == 0)
	{
		buf[--i] = '0';
		*start = &buf[i];
		return (1);
	}
	while (n != 0)
	{
		hex_temp = n % 16;
		buf[--i] = hex_buf[hex_temp];
		n /= 16;
	}
	*start = &buf[i];
	return ((size_t)(&buf[19] - *start));
}

static void	set_prefix(t_print *paper, uintptr_t arg)
{
	if (arg)
	{
		paper->prefix = "0x";
		paper->prefix_len = 2;
	}
}
//prefix is always "0x" even for NULL
