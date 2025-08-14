/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 16:56:50 by hnah              #+#    #+#             */
/*   Updated: 2025/08/11 10:30:57 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
//#include <stdio.h>

void	ft_putnbr(int nb)
{
	long	nb_long;
	char	buffer[11];
	int		i;

	nb_long = nb;
	if (nb_long == 0)
		write(1, "0", 1);
	if (nb_long < 0)
	{
		write(1, "-", 1);
		nb_long = -nb_long;
	}
	i = 0;
	while (nb_long > 0)
	{
		buffer[i++] = nb_long % 10 + '0';
		nb_long /= 10;
	}
	while (i > 0)
		write(1, &buffer[--i], 1);
}

/*int main(void)
{
	ft_putnbr(-2147483648);
}*/
