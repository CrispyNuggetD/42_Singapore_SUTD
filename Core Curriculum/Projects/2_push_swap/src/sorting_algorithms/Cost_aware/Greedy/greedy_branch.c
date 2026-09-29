/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   greedy_branch.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 15:56:51 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 17:49:18 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"

/*
** One branch owns these copies. The parent stacks never change.
** The child call finishes and returns a cost before this call continues.
** NULL disables recording, but executes the real rotation/push operations.
** At depth one, skip simulation unless the push empties B: alignment matters.
*/
static int	continuation_cost(circle_buf *a, circle_buf *b,
	t_greedy_search search, const t_greedy_plan *plan)
{
	circle_buf	copies[2];

	if (search.depth == 1 && cbuf_len(b) > 1)
	{
		debug_lookahead_stop(0, 0, "depth limit");
		return (0);
	}
	copies[A] = *a;
	copies[B] = *b;
	if (greedy_execute_plan(NULL, &copies[A], &copies[B], plan) == ERROR)
		return (-1);
	return (greedy_lookahead_cost(&copies[A], &copies[B], search.depth - 1,
			search.budget - plan->cost));
}

/* Each remaining insertion needs at least one pa. Equality cannot win ties. */
int	greedy_branch_cost(circle_buf *a, circle_buf *b,
	t_greedy_search search, const t_greedy_plan *plan)
{
	int	min_remaining;
	int	remaining;

	debug_lookahead_try(search.depth, cbuf_len(b), plan);
	min_remaining = search.depth - 1;
	ryker_ft_update_min(&min_remaining, cbuf_len(b) - 1);
	if (plan->cost + min_remaining >= search.budget)
	{
		debug_lookahead_pruned(search.depth, cbuf_len(b));
		return (GREEDY_PRUNED);
	}
	remaining = continuation_cost(a, b, search, plan);
	if (remaining < 0)
		return (remaining);
	return (plan->cost + remaining);
}
