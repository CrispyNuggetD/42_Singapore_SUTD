/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ryker_ft_max_variadic.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/02 11:04:37 by hnah              #+#    #+#             */
/*   Updated: 2026/10/02 11:04:37 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdarg.h>

int	ryker_ft_max_variadic(int count, ...)
{
	va_list	args;
	int		max;
	int		value;

	if (count <= 0)
		return (0);
	va_start(args, count);
	max = va_arg(args, int);
	count--;
	while (count > 0)
	{
		value = va_arg(args, int);
		if (value > max)
			max = value;
		count--;
	}
	va_end(args);
	return (max);
}
