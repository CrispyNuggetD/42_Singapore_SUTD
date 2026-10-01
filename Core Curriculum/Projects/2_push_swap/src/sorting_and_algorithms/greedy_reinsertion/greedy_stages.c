/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   greedy_stages.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:11:30 by hnah              #+#    #+#             */
/*   Updated: 2026/09/30 23:00:02 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"

/* Exactly one selector writes the plan; normalise its return convention. */
static int	choose_plan(t_circle_buf *a, t_circle_buf *b, int depth,
	t_greedy_path *best)
{
	if (depth == 0)
	{
		best->length = 1;
		return (greedy_choose_plan_local(a, b, &best->plans[0]));
	}
	if (greedy_choose_plan_lookahead(a, b, depth, best) < 0)
		return (ERROR);
	return (SUCCESS);
}

static int	greedy_insert_batch(t_soln *x, t_circle_buf stacks[2],
	int depth, int execute_limit)
{
	t_greedy_path	best;
	int				i;

	if (execute_limit < 1)
		return (ERROR);
	debug_search_start(cbuf_len(&stacks[B]), ryker_ft_max(1, depth));
	if (choose_plan(&stacks[A], &stacks[B], depth, &best) == ERROR)
		return (ERROR);
	i = 0;
	while (i < ryker_ft_min(execute_limit, best.length))
	{
		debug_greedy_execute(&best.plans[i]);
		if (greedy_execute_plan(x, &stacks[A], &stacks[B], &best.plans[i])
			== ERROR)
			return (ERROR);
		debug_insertion_done();
		i++;
	}
	return (SUCCESS);
}

/* Preparation leaves A circularly ascending; insertion preserves that. */
int	greedy_insert_all(t_soln *x, t_circle_buf stacks[2], int use_lookahead)
{
	int			is_a_lot;
	const int	depth[2] = {LOOKAHEAD_DEPTH_100, LOOKAHEAD_DEPTH_500};
	const int	execute_limit[2] = {EXECUTE_LIMIT_100, EXECUTE_LIMIT_500};

	if (use_lookahead != 0 && use_lookahead != 1)
		return (ERROR);
	is_a_lot = 0;
	if ((cbuf_len(&stacks[B]) + cbuf_len(&stacks[A])) > 100)
		is_a_lot = 1;
	while (cbuf_len(&stacks[B]) > 0)
	{
		if (!use_lookahead)
		{
			if (greedy_insert_batch(x, stacks, 0, 1) == ERROR)
				return (ERROR);
		}
		else
		{
			if (greedy_insert_batch(x, stacks, depth[is_a_lot],
					execute_limit[is_a_lot]) == ERROR)
				return (ERROR);
		}
	}
	return (SUCCESS);
}

int	greedy_reinsertion(t_soln *x, t_circle_buf stacks[2],
	t_algorithm algo)
{
	const t_algo_config	*config;

	config = algorithm_config(algo);
	if (!config || (stacks[A].capacity > 501 && algo != ALGO_THREE_LOCAL)
		|| greedy_prepare(x, &stacks[A], &stacks[B], config->seed) == ERROR)
		return (ERROR);
	debug_pass_start(cbuf_len(&stacks[B]), 1, algo);
	if (algo == ALGO_LIS_OPENING_ONE && cbuf_len(&stacks[B]) > 0)
	{
		if (greedy_insert_batch(x, stacks, LOOKAHEAD_DEPTH_FIRST_MOVE,
				1) == ERROR)
			return (ERROR);
	}
	else if (algo == ALGO_LIS_OPENING_BATCH && cbuf_len(&stacks[B]) > 0)
	{
		if (greedy_insert_batch(x, stacks, LOOKAHEAD_DEPTH_FIRST_MOVE,
				EXECUTE_LIMIT_FIRST_MOVE) == ERROR)
			return (ERROR);
	}
	if (greedy_insert_all(x, stacks, config->use_lookahead) == ERROR)
		return (ERROR);
	if (rot_a_min_to_top(x, &stacks[A]) == ERROR)
		return (ERROR);
	debug_status_end(x->step);
	return (SUCCESS);
}
