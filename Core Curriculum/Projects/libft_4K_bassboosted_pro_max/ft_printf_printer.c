/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_printer.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 02:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/01/09 02:08:19 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	send_for_printing(t_context *context, t_print *paper)
{
	if (paper->sign && write_guaranteed(context, &paper->sign, 1) < 0)
		return (-1);
	if (paper->prefix_len && write_guaranteed(context,
			paper->prefix, paper->prefix_len) < 0)
		return (-1);
	if (paper->core_len && write_guaranteed(context, paper->core,
			paper->core_len) < 0)
		return (-1);
	return (0);
}
