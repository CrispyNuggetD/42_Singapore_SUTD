/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_percent.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 09:34:40 by hnah              #+#    #+#             */
/*   Updated: 2026/01/07 13:17:41 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_printf_percent(t_context *context)
{
	t_print	paper;
	int		printer_len;

	(void)context;
	ft_printf_init_t_print(&paper);
	paper.core_len = 1;
	paper.core = "%";
	printer_len = ft_printf_print_config(context, &paper);
	return (printer_len);
}
