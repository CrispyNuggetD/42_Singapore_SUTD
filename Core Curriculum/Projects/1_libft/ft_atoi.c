/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 12:44:41 by hnah              #+#    #+#             */
/*   Updated: 2025/12/07 15:15:59 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// atoi supposed to return garbage on overflow
int	ft_atoi(const char *nptr)
{
	int	sign;
	int	number;

	number = 0;
	sign = 1;
	if (*nptr == '-')
	{
		sign = -1;
		nptr++;
	}
	else if (*nptr == '+')
	{
		sign = 1;
		nptr++;
	}
	while (*nptr && *nptr >= '0' && *nptr <= '9')
	{
		number = number * 10 + (*nptr - '0');
		nptr++;
	}
	return (sign * number);
}
