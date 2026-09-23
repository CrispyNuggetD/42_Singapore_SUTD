/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   greedy_reinsertion.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 18:03:22 by hnah              #+#    #+#             */
/*   Updated: 2026/09/23 23:15:32 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"

static int	greedy_scan_target(circle_buf *a, int desired_rank, int reverse)
{
	int	current_rank;
	int	index;
	int	prev_rank;

	index = 0;
	prev_rank = -1;
	while (cbuf_read_at(a, index, &current_rank) == SUCCESS
		&& (current_rank - desired_rank) * reverse < 0)
	{
		if (prev_rank != -1 && (current_rank - prev_rank) * reverse < 0)
			break ;
		prev_rank = current_rank;
		index += reverse;
	}
	return (index);
}

/* Return a logical insertion target without changing A. */
/* It is a bit cursed, but it works. */
int	greedy_find_target(circle_buf *a, int desired_rank, int *target_index)
{
	int	current_rank;
	int	index;
	int	reverse;

	reverse = 1;
	*target_index = GREEDY_NO_TARGET;
	if (cbuf_len(a) == 0)
		return (SUCCESS);
	if (cbuf_read_at(a, 0, &current_rank) == SUCCESS
		&& current_rank > desired_rank)
		reverse = -1;
	index = greedy_scan_target(a, desired_rank, reverse);
	if (index < 0)
		index += cbuf_len(a) + 1;
	if (index == cbuf_len(a))
		*target_index = 0;
	else
		*target_index = index;
	return (SUCCESS);
}

/*
** TODO: For one valid B index, find its target and compare all four routes.
** Remember top-index zero and empty A. Keep the winning signed rotations.
** Include the push in cost. Do not rotate stacks while evaluating a plan.
*/
int	greedy_plan_candidate(circle_buf *a, circle_buf *b, int b_index,
		t_greedy_plan *plan)
{
	int	b_candidate;
	int	target_index;
	int	other_cost;

	if (cbuf_read_at(b, b_index, &b_candidate) ||
		greedy_find_target(a, b_candidate, &target_index) == ERROR)
		return (ERROR);
	plan->candidate_rank = b_candidate;
	if (cbuf_len(a) == 0)
		plan->rot_a = 0;
	else
		plan->rot_a = target_index;
	plan->rot_b = b_index;
	plan->cost = ryker_ft_max(plan->rot_a, plan->rot_b) + 1;
	other_cost = ryker_ft_max((cbuf_len(a) - plan->rot_a), (cbuf_len(b) - plan->rot_b)) + 1;
	if (ryker_ft_update_min(&plan->cost, other_cost) == 1)
	{
		plan->rot_a = target_index - cbuf_len(a);
		plan->rot_b = b_index - cbuf_len(b);
	}
	other_cost = ryker_ft_max((target_index - cbuf_len(a)), (cbuf_len(b) - plan->rot_b) + 1;
	return (SUCCESS);
}

/*
** TODO: Evaluate each candidate in B, retaining the cheapest complete plan.
** First minimum wins ties for now. Empty B returns ERROR: no candidate.
** How will you initialise best without comparing uninitialised fields?
*/
int	greedy_choose_plan(circle_buf *a, circle_buf *b, t_greedy_plan *best)
{
	(void)a;
	(void)b;
	(void)best;
	return (ERROR);


	else if (target_index > (cbuf_len(a) / 2))
		plan->rot_a = target_index - cbuf_len(a);
	else
		plan->rot_a = target_index;
	if (cbuf_len(b) == 0)
		plan->rot_b = 0;
	else if (b_index > (cbuf_len(b) / 2))
		plan->rot_b = b_index - cbuf_len(b);
	else
		plan->rot_b = b_index;
}

/*
** TODO: Execute a fresh plan using existing operation wrappers and soln.
** Share compatible rotations, finish leftovers, then push. Check failures.
** Use local remaining counts so the caller's plan stays unchanged.
*/
int	greedy_execute_plan(soln *x, circle_buf *a, circle_buf *b,
		const t_greedy_plan *plan)
{
	(void)x;
	(void)a;
	(void)b;
	(void)plan;
	return (ERROR);
}
