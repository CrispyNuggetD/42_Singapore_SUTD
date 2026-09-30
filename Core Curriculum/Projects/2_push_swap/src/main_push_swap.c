/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_push_swap.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:41:55 by hnah              #+#    #+#             */
/*   Updated: 2026/09/30 17:35:47 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	free_and_error(soln *x)
{
	if (x)
	{
		ryker_ft_free_str_array(x->ans);
		free(x->ans_len);
	}
	ft_putendl_fd("Error", STDERR_FILENO);
	return (ERROR);
}

int	main(int argc, char **argv)
{
	int	i;
	int	count;
	circle_buf	a;
	circle_buf	b;
	soln	x;

	i = 1;
	count = 0;
	if (argc == 1 || argc > 501)
		return (ERR_INVALID_INPUT);
	while (i < argc)
	{
		if (count_int_in_str(argv[i++], &count, b.buf) == ERROR)
			return (free_and_error(NULL));
	}
	if (rank_values(count, b.buf, a.buf) == ERROR)
		return (free_and_error(NULL));
	cbuf_init_ab(&a, &b, count);
	if (soln_init(&x, ALGO_COUNT, MAX_MOVES_CONSIDERED) == ERROR)
		return (free_and_error(&x));
	if (solve(&x, &a, &b, count) == ERROR)
		return (free_and_error(&x));
	if (print_best_soln(&x) == ERROR)
		return (free_and_error(&x));
	debug_print_soln(&x, &a);
	/*
	printf("\n values: \n");
	debug_print_int_array(b.buf, count);
	printf("\n ranks: \n");
	debug_print_int_array(a.buf, count);
	cbuf_print_stacks(&a, &b);
	debug_print_soln(&x, &a);
	*/
	ryker_ft_free_str_array(x.ans);
	free(x.ans_len);
	return (SUCCESS);
}
