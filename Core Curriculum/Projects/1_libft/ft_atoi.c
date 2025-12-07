/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 12:44:41 by hnah              #+#    #+#             */
/*   Updated: 2025/12/07 13:34:22 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int atoi(const char *nptr)
{
	int		is_negative;
	long	number;

	number = 0;
	is_negative = 0;
	if (*nptr = '-')
	{
		is_negative = 1;
		nptr++;
	}
	else if (*nptr = '+')
	{       
		is_negative = 0;
		nptr++;
	}
	while (*nptr && *nptr >= '0' && *nptr <= '9')
	{
		number = number * 10 + (*nptr - '0');
		nptr++;
	}
return (number);
}
