/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   greedy_choose_plan.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 17:47:39 by hnah              #+#    #+#             */
/*   Updated: 2026/09/30 14:01:20 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"
#include <limits.h>

/*
** Evaluate each candidate in B, retaining the cheapest complete plan.
** First minimum wins ties for now. Empty B returns ERROR: no candidate.
*/
int	greedy_choose_plan_local(t_circle_buf *a, t_circle_buf *b,
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

/* The public selector starts with no incumbent: INT_MAX is only a bound. */
int	greedy_choose_plan_lookahead(t_circle_buf *a, t_circle_buf *b, int depth,
	t_greedy_path *best_path)
{
	t_greedy_search	search;

	if (!a || !b || !best_path || depth < 1 || cbuf_len(b) == 0)
		return (-1);
	search.depth = depth;
	search.budget = INT_MAX;
	search.best_path = best_path;
	return (greedy_choose_bounded(a, b, search));
}

/*
** Plan, evaluate and report one candidate; pruned is not an error.
** TODO: collect the winning child continuation after plans[0].
*/
static int	evaluate_candidate(t_circle_buf *a, t_circle_buf *b,
	t_greedy_search search, t_greedy_path *candidate_path)
{
	int	score;

	candidate_path->length = 1;
	score = greedy_branch_cost(a, b, search, candidate_path);
	if (score != -1)
		debug_lookahead_result(search.depth, &candidate_path->plans[0],
			score, search.budget);
	return (score);
}

/*
** Try siblings one at a time; retain only their best cost and first plan.
** Each branch starts from the same parent state. No search tree is allocated.
** The caller executes only best_path->plans[0], then searches again.
** A child receives its parent's budget minus the immediate insertion cost.
*/
int	greedy_choose_bounded(t_circle_buf *a, t_circle_buf *b,
	t_greedy_search search)
{
	t_greedy_path	candidate_path;
	int				index;
	int				score;
	int				best_score;

	ft_memset(&candidate_path, 0, sizeof(candidate_path));
	best_score = GREEDY_PRUNED;
	index = -1;
	while (++index < cbuf_len(b))
	{
		if (greedy_plan_candidate(a, b, index, candidate_path.plans) == ERROR)
			return (-1);
		score = evaluate_candidate(a, b, search, &candidate_path);
		if (score == -1)
			return (-1);
		if (score >= 0)
		{
			search.budget = score;
			best_score = score;
			*search.best_path = candidate_path;
		}
	}
	return (best_score);
}
