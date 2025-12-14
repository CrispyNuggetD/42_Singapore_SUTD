/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/14 15:05:29 by hnah              #+#    #+#             */
/*   Updated: 2025/12/14 15:06:13 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putnbr_fd(int n, int fd)
{
	char	str[11];
	int		is_negative;
	long	n_long;
	int		i;
	
	if (fd < 0)
		return ;
	i = 0;	
	is_negative = (n < 0);
	n_long = (long)n;
	if (is_negative)
		n_long = -n_long;
	if (n_long == 0)
		str[i++] = '0';
	while (n_long > 0)
	{
		str[i++] = '0' + (n_long % 10);
		n_long /= 10;
	}
	i--;
	if (is_negative)
		write(fd, &"-", 1);
	while (i >= 0)
		ft_putchar_fd(str[i--], fd);
	return ;
}

/* int	main(void)
{
	ft_putnbr_fd(31, 1);
}  */

// I can't use ft_itoa helper cos it'll then have to be freed (illegal).
