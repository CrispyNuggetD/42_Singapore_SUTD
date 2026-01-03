/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_helpers.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 12:17:39 by hnah              #+#    #+#             */
/*   Updated: 2026/01/03 13:40:57 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

// Close va_list and return error
int	ft_printf_error_end_stream(va_list *input)
{
	va_end(*input);
	return (-1);
}

void	init_t_print(t_print *paper)
{
	*paper = (t_print){0};
	paper -> pad_char = ' ';
}
/* 
Compound literal (t_print){0} creates a temporary t_print value.
Because of {0}, the first field is init 0, 
and all the rest become 0 too.

This simplistic init is same as:
static void	init_t_print(t_print *paper)
{
	paper -> sign = 0;
	paper -> prefix = NULL;
	paper -> prefix_len = 0;
	paper -> core = NULL;
	paper -> core_len = 0;
	paper -> prec_zeros = 0;
	paper -> pad_len = 0;
	paper -> pad_char = ' ';
	paper -> left = 0;
}
*/