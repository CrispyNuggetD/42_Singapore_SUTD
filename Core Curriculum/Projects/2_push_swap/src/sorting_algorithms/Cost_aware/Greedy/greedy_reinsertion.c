/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   greedy_reinsertion.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 18:03:22 by hnah              #+#    #+#             */
/*   Updated: 2026/09/25 18:26:21 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"

static int	greedy_scan_target(circle_buf *a, int desired_rank, int scan_dir)
{
	int	current_rank;
	int	index;
	int	prev_rank;

	index = 0;
	prev_rank = -1;
	while (cbuf_read_at(a, index, &current_rank) == SUCCESS
		&& (current_rank - desired_rank) * scan_dir < 0)
	{
		if (prev_rank != -1 && (current_rank - prev_rank) * scan_dir < 0)
			break ;
		prev_rank = current_rank;
		index += scan_dir;
	}
	return (index);
}

/* Return a logical insertion target without changing A. */
/* It is a bit cursed, but it works. */
int	greedy_find_target(circle_buf *a, int desired_rank, int *target_index)
{
	int	current_rank;
	int	index;
	int	scan_direction;

	scan_direction = 1;
	*target_index = GREEDY_NO_TARGET;
	if (cbuf_len(a) == 0)
		return (SUCCESS);
	if (cbuf_read_at(a, 0, &current_rank) == SUCCESS
		&& current_rank > desired_rank)
		scan_direction = -1;
	index = greedy_scan_target(a, desired_rank, scan_direction);
	if (index < 0)
		index += cbuf_len(a) + 1;
	if (index == cbuf_len(a))
		*target_index = 0;
	else
		*target_index = index;
	return (SUCCESS);
}

static void	greedy_update_best_cost(int candidate_costs[4], int rev_cost[2],
	t_greedy_plan *plan)
{
	int	current_candidate;
	int	best_candidate;

	current_candidate = 1;
	best_candidate = 0;
	while (current_candidate < 4)
	{
		if (ryker_ft_update_min(&plan->cost,
				candidate_costs[current_candidate]))
			best_candidate = current_candidate;
		current_candidate++;
	}
	if (best_candidate == 1)
		plan->rot_b = -rev_cost[1];
	else if (best_candidate == 2)
		plan->rot_a = -rev_cost[0];
	else if (best_candidate == 3)
	{
		plan->rot_a = -rev_cost[0];
		plan->rot_b = -rev_cost[1];
	}
	return ;
}

/*
** For one valid B index, find its target and compare all four routes.
** Included the push in cost.
*/
int	greedy_plan_candidate(circle_buf *a, circle_buf *b, int b_index,
	t_greedy_plan *plan)
{
	int	target_index;
	int	candidate_costs[4];
	int	rev_cost[2];

	if (cbuf_read_at(b, b_index, &plan->candidate_rank)
		|| greedy_find_target(a, plan->candidate_rank, &target_index) == ERROR)
		return (ERROR);
	if (cbuf_len(a) == 0)
		plan->rot_a = 0;
	else
		plan->rot_a = target_index;
	plan->rot_b = b_index;
	rev_cost[0] = cbuf_rev_moves(a, plan->rot_a);
	rev_cost[1] = cbuf_rev_moves(b, plan->rot_b);
	candidate_costs[0] = ryker_ft_max(plan->rot_a, plan->rot_b) + 1;
	candidate_costs[1] = plan->rot_a + rev_cost[1] + 1;
	candidate_costs[2] = rev_cost[0] + plan->rot_b + 1;
	candidate_costs[3] = ryker_ft_max(rev_cost[0], rev_cost[1]) + 1;
	plan->cost = candidate_costs[0];
	greedy_update_best_cost(candidate_costs, rev_cost, plan);
	return (SUCCESS);
}

/*
** Evaluate each candidate in B, retaining the cheapest complete plan.
** First minimum wins ties for now. Empty B returns ERROR: no candidate.
*/
int	greedy_choose_plan(circle_buf *a, circle_buf *b, t_greedy_plan *best)
{
	t_greedy_plan	candidate_plan;
	int				b_index;
	int				b_len;

	b_len = cbuf_len(b);
	if (b_len == 0)
		return (ERROR);
	b_index = 0;
	while (b_index < b_len)
	{
		if (greedy_plan_candidate(a, b, b_index, &candidate_plan) == ERROR)
			return (ERROR);
		if (b_index == 0 || candidate_plan.cost < best->cost)
			*best = candidate_plan;
		b_index++;
	}
	return (SUCCESS);
}
