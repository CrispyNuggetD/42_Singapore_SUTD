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

static int	run_solver(t_soln *x, t_circle_buf stacks[2])
{
	int	count;
	int	slots;

	count = cbuf_len(&stacks[A]);
	slots = ALGO_COUNT;
	if (count > 500)
		slots = 1;
	if (soln_init(x, slots, MAX_MOVES_CONSIDERED))
		return (ERROR);
	if (solve(x, &stacks[A], &stacks[B], count) || print_best_soln(x))
		return (ERROR);
	if (count <= 500)
		debug_print_soln(x, &stacks[A]);
	return (SUCCESS);
}

int	main(int argc, char **argv)
{
	t_circle_buf	stacks[2];
	t_soln			x;
	int				status;

	if (argc == 1)
		return (SUCCESS);
	ft_bzero(&x, sizeof(x));
	stacks[A].large_buf = NULL;
	stacks[B].large_buf = NULL;
	status = parse_input(argv + 1, &stacks[A], &stacks[B]);
	if (status == SUCCESS)
		status = run_solver(&x, stacks);
	ryker_ft_free_str_array(x.ans);
	free(x.ans_len);
	free(stacks[A].large_buf);
	free(stacks[B].large_buf);
	if (status == ERROR)
		ft_putendl_fd("Error", STDERR_FILENO);
	return (status);
}
