/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   solve.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:11:30 by hnah              #+#    #+#             */
/*   Updated: 2026/09/30 20:16:45 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

/* Each candidate owns a fresh copy; preparation never resets a solution. */
int	solve(t_soln *x, t_circle_buf *a, t_circle_buf *b, int count)
{
	t_circle_buf	stacks[2];
	t_algorithm		algo;

	if (count != cbuf_len(a) || cbuf_len(b) != 0)
		return (ERROR);
	if (count <= BRUTE_MAX_N)
	{
		if (new_soln_init(x, stacks, a, b) == ERROR)
			return (ERROR);
		if (count >= 1 && count <= 4
			&& get_precomputed_bfs(x, &stacks[A], count) == ERROR)
			return (ERROR);
		if ((count < 1 || count > 4)
			&& brute_solve(x, &stacks[A], &stacks[B], count) == ERROR)
			return (ERROR);
		if (SKIP_OTHER_ALGO_AFTER_BFS)
			return (SUCCESS);
	}
	algo = ALGO_LIS_LOCAL;
	/* BFS already has its own dispatch above; only run greedy entries here. */
	while (algo < ALGO_COUNT)
	{
		if (new_soln_init(x, stacks, a, b) == ERROR)
			return (ERROR);
		if (greedy_reinsertion(x, stacks, algo) == ERROR)
			return (ERROR);
		algo++;
	}
	return (SUCCESS);
}
