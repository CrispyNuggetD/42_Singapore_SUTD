/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi_base.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 13:16:03 by hnah              #+#    #+#             */
/*   Updated: 2025/08/11 16:34:14 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
//#include <stdio.h>

int	atoi_base(char *str, char *base, int base_len)
{
	int	sign;
	int	result;
	int	i;

	sign = 1;
	result = 0;
	while (*str && (*str == ' ' || (*str >= 9 && *str <= 13)))
		str++;
	while (*str && (*str == '+' || *str == '-'))
	{
		if (*str == '-')
			sign *= -1;
		str++;
	}
	while (*str)
	{
		i = 0;
		while (base[i] && base[i] != *str)
			i++;
		result = result * base_len + i;
		str++;
	}
	return (sign * result);
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

int	ft_atoi_base(char *str, char *base)
{
	int	result;
	int	base_len;
	int	i;

	result = 0;
	if (check_invalid(base))
		return (0);
	else
	{
		base_len = 0;
		i = 0;
		while (base[i++])
			base_len += 1;
		result = atoi_base(str, base, base_len);
	}
	return (result);
}

/*int	main(void)
{
	char	*base = "01";
	ft_atoi_base("101101", base);
}*/
