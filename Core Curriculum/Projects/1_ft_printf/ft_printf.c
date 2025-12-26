/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 13:19:21 by hnah              #+#    #+#             */
/*   Updated: 2025/12/26 10:02:47 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include <stdio.h>

int	ft_printf(const char *input, ...)
{
	int					printer;
	static t_handler	g_handlers[256];

	printer = 0;
	(void)input;
	return (printer);
}
//type_handler global_handler

static void	init_handlers(void)
{
	g_handlers[(unsigned char)('i')] ||
		g_handlers[(unsigned char)('d')] = print_d_i;
}

static void	dispatch_key(int key)
{
	t_handler	fn;
	
	if (key < 0 || key > 255)
		return;
	fn = g_handlers[(unsigned char)key];
	if (fn)
		fn();
}


int	main(void)
{
	dispatch_key('i');
	return (0);
}
// printf("Test ft_itoa(31)\nResult is: %s", ft_itoa(31));
