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
	int		success_status;
	
	success_status = SUCCESS;
	str_moving = (char *)str;
	while (*str_moving)
	{
		if (ryker_ft_isspace(*str_moving))
			str_moving++;
		if (ft_isdigit(*str_moving))
		{
			count++;
			values[count-1] = ft_atoi(str_moving);
			printf("Cur num: %i\n", values[count-1]);
			while (ft_isdigit(*str_moving))
				str_moving++;
		}
	}
	return (success_status);
}

int	main(int argc, char **argv)
{
	int count;
	int	values[500];
	
	count = 0;
	if (argc < 2)
		return (ERR_INVALID_INPUT);
	while(count < argc)
		if(count_int_in_str(argv[count + 1], &count, &values[count]) == ERROR)
		return (ERROR);
	printf("%i\n", count);
	return (SUCCESS);
}