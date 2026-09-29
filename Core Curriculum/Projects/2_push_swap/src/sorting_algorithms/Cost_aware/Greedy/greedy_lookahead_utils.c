/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   greedy_lookahead_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 15:56:51 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 17:49:18 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"

/* B is empty: all ranks are in A, so the O(1) alignment contract holds. */
static int	alignment_cost(circle_buf *a)
{
	int	rotations;

	if (rot_a_min_plan(a, &rotations) == ERROR)
		return (-1);
	return (ryker_ft_abs(rotations));
}

/* A leaf returns a cost without executing another insertion. */
static int	leaf_cost(int depth, int cost, char *reason, int budget)
{
	debug_lookahead_stop(depth, cost, reason);
	if (cost < 0)
		return (-1);
	if (cost >= budget)
		return (GREEDY_PRUNED);
	return (cost);
}

/*
** depth counts insertions, not individual rotations. It shrinks per branch.
** Check completion first: even at depth zero, finishing alignment has a cost.
** At an unfinished depth-zero leaf we stop looking, so return zero.
*/
int	greedy_lookahead_cost(circle_buf *a, circle_buf *b, int depth,
	int budget)
{
	t_greedy_plan	unused_plan;
	t_greedy_search	search;

	if (!a || !b || depth < 0)
		return (-1);
	if (cbuf_len(b) == 0)
		return (leaf_cost(depth, alignment_cost(a), "B empty: alignment",
				budget));
	if (depth == 0)
		return (leaf_cost(depth, 0, "depth limit", budget));
	search.depth = depth;
	search.budget = budget;
	search.best_first_plan = &unused_plan;
	return (greedy_choose_bounded(a, b, search));
}
