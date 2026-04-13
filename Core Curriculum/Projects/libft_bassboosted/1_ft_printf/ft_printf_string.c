/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_string.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 13:55:30 by hnah              #+#    #+#             */
/*   Updated: 2026/01/08 12:00:58 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_printf_string(t_context *context)
{
	char	*arg;
	size_t	len;
	t_print	paper;

	ft_printf_init_t_print(&paper);
	arg = va_arg(*(context->input), char *);
	if (!arg)
	{
		if ((context->spec->flags & FLAG_PREC) && context->spec->precision >= 6)
			arg = "(null)";
		else if ((context->spec->flags & FLAG_PREC))
			arg = "";
		else
			arg = "(null)";
	}
	paper.core = arg;
	len = ft_strlen(arg);
	if ((context->spec->flags & FLAG_PREC) && context->spec->precision >= \
0 && (size_t)context->spec->precision < len)
		len = context->spec->precision;
	paper.core_len = len;
	return (ft_printf_print_config(context, &paper));
}
