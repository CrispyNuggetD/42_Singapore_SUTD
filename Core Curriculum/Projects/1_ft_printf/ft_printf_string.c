/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_string.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 13:55:30 by hnah              #+#    #+#             */
/*   Updated: 2026/01/07 14:33:35 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_printf_string(t_context *context)
{
	int		printer_len;
	char	*arg;
	t_print	paper;
	
	ft_printf_init_t_print(&paper);
	arg = va_arg(*(context -> input), char *);
	if (!arg)
		paper.core = "(null)";
	paper.core = arg;
	// do precision to trim
	paper.core_len = ft_strlen(paper.core);
	printer_len = ft_printf_print_config(context, &paper);
	return (printer_len);
}
