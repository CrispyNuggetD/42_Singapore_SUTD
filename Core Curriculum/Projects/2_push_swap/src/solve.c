/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   solve.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:11:30 by hnah              #+#    #+#             */
/*   Updated: 2026/09/28 23:17:50 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	solve(soln *x, circle_buf *a_ori, circle_buf *b_ori, int count)
{
	char keep_flags[500];
	circle_buf	stacks[2];

	if (0)
		return (debug_hidden_bfs(x, &stacks[A], &stacks[B]));
	if (count <= BRUTE_MAX_N)
	{
		new_soln_init(x, stacks, a_ori, b_ori);
		printf("sizeof(t_brutestate) = %zu\n", sizeof(t_brutestate));
		printf("sizeof(all t_brutestate) = %zu\n", sizeof(t_brutestate) * BRUTE_TOTAL_N_PLUS_1_FACTORIAL);
		printf("KB sizeof(all t_brutestate) = %zu\n", sizeof(t_brutestate) * BRUTE_TOTAL_N_PLUS_1_FACTORIAL / 1000);
		printf("MB sizeof(all t_brutestate) = %zu\n", sizeof(t_brutestate) * BRUTE_TOTAL_N_PLUS_1_FACTORIAL / 1000000);
		
		if (brute_solve(x, &stacks[A], &stacks[B]) == ERROR)
			return (ERROR);
	}
	else //check if b is empty and a is less than 3-5 items later.
	{
		new_soln_init(x, stacks, a_ori, b_ori);
		if (greedy_reinsertion(x, &stacks[A], &stacks[B], NULL) == ERROR)
			return (ERROR);
		new_soln_init(x, stacks, a_ori, b_ori);
		ft_memset(&keep_flags, 0, sizeof(keep_flags));
		if (cbuf_lis(&stacks[A], keep_flags)
			|| greedy_reinsertion(x, &stacks[A], &stacks[B], keep_flags) == ERROR)
			return (ERROR);
	}
	return (SUCCESS);
}

/*
	if (a_size == -1)
		a_size = cbuf_len(a);
	if (recur == -1)
		recur = a_size;
	if ((recur == 2 || a_size == 2) && (larger_top(x, a)))
		return (sa(x, a));
*/
