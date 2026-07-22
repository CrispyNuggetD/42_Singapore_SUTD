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
//printf("Cur num: %i\nCur index: %i\n", values[count-1], count-1);


int free_and_error()
{
	ft_putendl_fd("Error", 2);
	return (ERROR);
}


int	main(int argc, char **argv)
{
	int	i;
	int	count;
	int	values[500];
	
	count = 0;
	i = 1;
	if (argc == 1 || argc + 1 > 500)
		return (ERR_INVALID_INPUT);
	while (i < argc)
	{
		if (count_int_in_str(argv[++i], &count, values) == ERROR)
			return (free_and_error());
	}
	return (SUCCESS);
}