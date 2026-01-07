/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_print_config.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 02:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/01/07 22:01:13 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_printf_print_config(t_context *context, t_print *paper)
{
	size_t	content_len;
	int		sign_len;

	sign_len = (paper->sign != 0);
	if ((context->spec->flags & FLAG_PREC) && \
context->spec->precision > 0 && \
(size_t)context->spec->precision > paper->core_len)
		paper->prec_zeros = (size_t)context->spec->precision - \
paper->core_len;
	content_len = (size_t)sign_len + paper->prefix_len + \
paper->prec_zeros + paper->core_len;
	if (context->spec->width > 0 && \
(size_t)context->spec->width > content_len)
		paper->pad_len = (size_t)context->spec->width - content_len;
	if (context->spec->flags & FLAG_ZERO)
		paper->pad_char = '0';
	return (send_for_printing(context, paper));
}

/*
Ignored-when-irrelevant:
+ / space only make sense for signed conversions (d/i).
 They’re ignored for u, x, X, p.
# matters for x/X (and some others in full libc printf), 
but is ignored for plain decimal d/i.
Precision for %c is typically ignored; precision for %s 
limits max chars printed; precision for integers is “minimum digits”.
*/

/*
3) Decide ownership (important!)

t_print should not own memory. It’s just “here’s what to print”.
So: if core comes from malloc (ft_itoa), the handler 
frees it after ft_print_printer() returns.
*/