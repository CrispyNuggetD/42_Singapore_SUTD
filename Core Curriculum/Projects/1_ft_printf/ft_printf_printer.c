/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_printer.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 02:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/01/03 13:46:21 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_print_printer(t_context *context, const t_print *paper)
{
	(void)context;
	(void)paper;
	return (2);
}

/*
Ignored-when-irrelevant:
+ / space only make sense for signed conversions (d/i). They’re ignored for u, x, X, p.
# matters for x/X (and some others in full libc printf), but is ignored for plain decimal d/i.
Precision for %c is typically ignored; precision for %s limits max chars printed; precision for integers is “minimum digits”.
*/

/*
3) Decide ownership (important!)

t_print should not own memory. It’s just “here’s what to print”.
So: if core comes from malloc (ft_itoa), the handler frees it after ft_print_printer() returns.
*/