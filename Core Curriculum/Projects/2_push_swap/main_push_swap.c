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

int	free_and_error(void)
{
	ft_putendl_fd("Error", STDERR_FILENO);
	return (ERROR);
}

int	main(int argc, char **argv)
{
	int	i;
	int	count;
	cbuf	a;
	cbuf	b;
	soln	x;

	i = 1;
	count = 0;
	if (argc == 1 || argc + 1 > 500)
		return (ERR_INVALID_INPUT);
	while (i < argc)
	{
		if (count_int_in_str(argv[i++], &count, b.buf) == ERROR)
			return (free_and_error());
	}
	if (count < 2)
		return (free_and_error());
	if (rank_values(count, b.buf, a.buf) == ERROR)
		return (free_and_error());
	cbuf_info(&a, &b, count);
	if (soln_init(&x, 10, BUBBLE_SORT_MAX_500) == ERROR)
		return (free_and_error());
	if (solve(&x, &a, &b) == ERROR)
		return (free_and_error());
	if (print_first_soln(&x) == ERROR)
		return (free_and_error());
	/*
	printf("\n values: \n");
	debug_print_int_array(b.buf, count);
	printf("\n ranks: \n");
	debug_print_int_array(a.buf, count);
	cbuf_print_stacks(&a, &b);
	debug_print_soln(&x);
	*/
	return (SUCCESS);
}
