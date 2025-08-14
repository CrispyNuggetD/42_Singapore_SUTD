/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_base.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 13:16:03 by hnah              #+#    #+#             */
/*   Updated: 2025/08/11 15:14:07 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>

void	ft_putnbr(int nbr, char *base)
{
	long	nb_long;
	int		buffer[33];
	int		i;
	int		base_len;

	base_len = 0;
	i = 0;
	while (base[i++])
		base_len += 1;
	nb_long = nbr;
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
		buffer[i++] = nb_long % base_len;
		nb_long /= base_len;
	}
	while (i > 0)
		write(1, &base[buffer[--i]], 1);
}

int	check_invalid(char *base)
{
	int	i;
	int	j;

	i = 0;
	if (!base[i] || !base[i + 1])
		return (1);
	while (base[i])
	{
		j = i + 1;
		while (base[j])
		{
			if (base[i] == base[j] || base[i] == '+'
				|| base[i] == '-' || base[j] == '+' || base[j] == '-')
				return (1);
			else
				j++;
		}
		i++;
	}
	return (0);
}

void	ft_putnbr_base(int nbr, char *base)
{
	if (check_invalid(base))
		return ;
	else
		ft_putnbr(nbr, base);
}

/*int	main(void)
{
	char	*base = "0123456789ABCEDF";
	int		nbr = -2147483648;
	ft_putnbr_base(nbr, base);
}*/
