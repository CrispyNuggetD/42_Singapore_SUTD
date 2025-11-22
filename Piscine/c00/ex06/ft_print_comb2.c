/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_comb2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 15:23:34 by hnah              #+#    #+#             */
/*   Updated: 2025/07/24 16:04:53 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

void	print_two_digits(char c)
{
	write(1, &c, 1);
}

void	ft_print_comb2(void)
{
	int	a;
	int	b;

	a = 0;
	while (a <= 99)
	{
		b = a + 1;
		while (b <= 99)
		{
			print_two_digits(48 + a / 10);
			print_two_digits(48 + a % 10);
			write(1, " ", 1);
			print_two_digits(48 + b / 10);
			print_two_digits(48 + b % 10);
			if (a != 98)
				write(1, ", ", 2);
			b++;
		}
		a++;
	}
}
