/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_comb.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 13:56:38 by hnah              #+#    #+#             */
/*   Updated: 2025/07/25 16:00:48 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

// Exercise asks for mathematical combinations

void	ft_print_comb(void)
{
	char	x;
	char	y;
	char	z;

	x = 48;
	while (x <= 55)
	{
		y = x + 1;
		while (y <= 56)
		{
			z = y + 1;
			while (z <= 57)
			{
				write(1, &x, 1);
				write(1, &y, 1);
				write(1, &z, 1);
				if (x != 55)
					write(1, ", ", 2);
				z++;
			}
			y++;
		}
		x++;
	}
}
