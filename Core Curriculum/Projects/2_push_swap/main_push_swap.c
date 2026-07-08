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

int count_int_in_str(char *str)
{
	int count = 0;
	int i = 0;

	while (str[i])
	{
		if (ft_isdigit(str[i]))
		{
			count++;
			while (ft_isdigit(str[i]))
				i++;
		}
		i++;
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