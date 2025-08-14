/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 16:05:52 by hnah              #+#    #+#             */
/*   Updated: 2025/07/24 17:52:55 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

// nb_long is to prevent nb=-nb int overflow
void	ft_putnbr(int nb)
{
	char	digits_container[10];
	int		i;
	long	nb_long;

	nb_long = nb;
	if (nb_long < 0)
	{
		write(1, "-", 1);
		nb_long = -nb_long;
	}
	if (nb_long == 0)
		write(1, "0", 1);
	i = 0;
	while (nb_long > 0)
	{
		digits_container[i++] = nb_long % 10 + '0';
		nb_long /= 10;
	}
	while (i >= 1)
	{
		write(1, &digits_container[i - 1], 1);
		i--;
	}
}
