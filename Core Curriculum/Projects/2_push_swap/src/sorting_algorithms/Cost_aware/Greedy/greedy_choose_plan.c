/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   greedy_choose_plan.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 17:47:39 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 18:05:34 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"

/*
** Evaluate each candidate in B, retaining the cheapest complete plan.
** First minimum wins ties for now. Empty B returns ERROR: no candidate.
*/
int	greedy_choose_plan_local(circle_buf *a, circle_buf *b,
		t_greedy_plan *best_first_plan)
{
	t_greedy_plan	candidate_plan;
	int				b_index;
	int				best_score;

	if (cbuf_len(b) == 0)
		return (ERROR);
	b_index = -1;
	best_score = -1;
	while (++b_index < cbuf_len(b))
	{
		if (greedy_plan_candidate(a, b, b_index, &candidate_plan) == ERROR)
			return (ERROR);
		debug_lookahead_try(1, cbuf_len(b), &candidate_plan);
		debug_lookahead_result(1, &candidate_plan,
			candidate_plan.cost, best_score);
		if (best_score < 0 || candidate_plan.cost < best_score)
		{
			*best_first_plan = candidate_plan;
			best_score = candidate_plan.cost;
		}
	}
	return (SUCCESS);
}

/*
** One branch owns these copies. The parent stacks never change.
** The child call finishes and returns a cost before this call continues.
** NULL disables recording, but executes the real rotation/push operations.
*/
static int	branch_cost(circle_buf *a, circle_buf *b, int depth,
	const t_greedy_plan *plan)
{
	circle_buf	copies[2];
	int			remaining;

	debug_lookahead_try(depth, cbuf_len(b), plan);
	copies[A] = *a;
	copies[B] = *b;
	if (greedy_execute_plan(NULL, &copies[A], &copies[B], plan) == ERROR)
		return (-1);
	remaining = greedy_lookahead_cost(&copies[A], &copies[B], depth - 1);
	if (remaining < 0)
		return (-1);
	return (plan->cost + remaining);
}

/*
** Try siblings one at a time; retain only their best cost and first plan.
** Each branch starts from the same parent state. No search tree is allocated.
** The caller executes only *best_first_plan on real stacks, then searches again.
*/
int	greedy_choose_plan_lookahead(circle_buf *a, circle_buf *b, int depth,
	t_greedy_plan *best_first_plan)
{
	t_greedy_plan	candidate;
	int				index;
	int				score;
	int				best_score;

	if (!a || !b || !best_first_plan || depth < 1 || cbuf_len(b) == 0)
		return (-1);
	best_score = -1;
	index = -1;
	while (++index < cbuf_len(b))
	{
		if (greedy_plan_candidate(a, b, index, &candidate) == ERROR)
			return (-1);
		score = branch_cost(a, b, depth, &candidate);
		if (score < 0)
			return (-1);
		debug_lookahead_result(depth, &candidate, score, best_score);
		if (best_score < 0 || score < best_score)
		{
			best_score = score;
			*best_first_plan = candidate;
		}
	}
	return (best_score);
}
