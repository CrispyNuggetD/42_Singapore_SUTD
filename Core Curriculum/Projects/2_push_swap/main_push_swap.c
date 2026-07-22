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

int count_int_in_str(const char *str, int *count, int *values)
{
	char	*str_moving;
	
	str_moving = (char *)str;
	if ((*str_moving && ft_isdigit(*str_moving)) || *str_moving == '+' || *str_moving == '-')
	{
		*values = ft_atoi(str_moving);
		while (*str_moving == '+' || *str_moving == '-')
			str_moving++;
		while (*str_moving && ft_isdigit(*str_moving))
			str_moving++;	
		if (!*str_moving)
		{
			(*count)++;
			return (SUCCESS);
		}
	}
	return (ERROR);
}

int free_and_error()
{
	ft_putendl_fd("Error", 2);
	return (ERROR);
}

int	main(int argc, char **argv)
{
	int count;
	int	values[500];
	
	count = 0;
	if (argc < 2)
		return (ERR_INVALID_INPUT);
	while(count + 1 < argc)
	{
		if(count_int_in_str(argv[count + 1], &count, &values[count]) == ERROR)
			return (free_and_error());
		printf("Cur num: %i\n", values[count-1]);
	}
	printf("%i\n", count);
	return (SUCCESS);
}