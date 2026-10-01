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

/* BFS owns the first small-input candidate; other entries use greedy. */
static int	solve_small_bfs(t_soln *x, t_circle_buf stacks[2], int count)
{
	if (count >= 1 && count <= 4)
	{
		if (get_precomputed_bfs(x, &stacks[A], count))
			return (ERROR);
		debug_bfs_end(x->step);
		return (SUCCESS);
	}
	return (brute_solve(x, &stacks[A], &stacks[B], count));
}

/* Borrow heap buffers for one local pass; never recurse or free here. */
static int	solve_large_local(t_soln *x, t_circle_buf *a, t_circle_buf *b)
{
	t_circle_buf	stacks[2];
	int				status;

	x->cur = 0;
	x->step = 0;
	x->ans_len[0] = 0;
	stacks[A] = *a;
	stacks[B] = *b;
	status = greedy_reinsertion(x, stacks, ALGO_THREE_LOCAL);
	*a = stacks[A];
	*b = stacks[B];
	return (status);
}

/* Small candidates own inline copies; large inputs run one local pass. */
int	solve(t_soln *x, t_circle_buf *a, t_circle_buf *b, int count)
{
	t_circle_buf	stacks[2];
	t_algorithm		algo;

	if (count != cbuf_len(a) || cbuf_len(b) != 0)
		return (ERROR);
	if (count > 500)
		return (solve_large_local(x, a, b));
	if (count <= BRUTE_MAX_N)
	{
		if (new_soln_init(x, stacks, a, b) || solve_small_bfs(x, stacks, count))
			return (ERROR);
		if (SKIP_OTHER_ALGO_AFTER_BFS)
			return (SUCCESS);
	}
	algo = ALGO_THREE_LOCAL;
	while (algo < ALGO_COUNT && (ENABLE_OPENING_LOOKAHEAD
			|| algo < ALGO_LIS_OPENING_ONE))
	{
		if (new_soln_init(x, stacks, a, b)
			|| greedy_reinsertion(x, stacks, algo))
			return (ERROR);
		algo++;
	}
	return (SUCCESS);
}
