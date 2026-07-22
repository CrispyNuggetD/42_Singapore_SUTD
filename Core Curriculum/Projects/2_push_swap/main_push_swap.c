/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_push_swap.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:41:55 by hnah              #+#    #+#             */
/*   Updated: 2026/07/08 18:15:16 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int count_int_in_str(const char *str)
{
	int	count = 0;
	int	values[500];
	char	*str_moving;
	
	str_moving = (char *)str;
	while (*str_moving)
	{
		if (ryker_ft_isspace(*str_moving))
			str_moving++;
		if (ft_isdigit(*str_moving))
		{
			count++;
			values[count-1] = ft_atoi(*str_moving);
			while (ft_isdigit(*str_moving)
				str_moving++;
		}
	}
	return (count);
}

int	main(int argc, char **argv)
{
	int count = 0;
	
	if (argc < 2)
		return (ERR_INVALID_INPUT);
	while(argc-- > 1)
		count += count_int_in_str(argv[argc]);
	printf("%i\n", count);
	return (SUCCESS);
}