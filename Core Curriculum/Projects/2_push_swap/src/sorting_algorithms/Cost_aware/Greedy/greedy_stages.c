/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   greedy_stages.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:11:30 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 19:55:46 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"

/* Exactly one selector writes the plan; normalise its return convention. */
static int	choose_plan(circle_buf *a, circle_buf *b, int depth,
	t_greedy_plan *best)
{
	if (depth == 0)
		return (greedy_choose_plan_local(a, b, best));
	if (greedy_choose_plan_lookahead(a, b, depth, best) < 0)
		return (ERROR);
	return (SUCCESS);
}

/* Preparation leaves A circularly ascending; insertion preserves that. */
int	greedy_insert_all(soln *x, circle_buf *a, circle_buf *b, int depth)
{
	t_greedy_plan	best;

	if (depth < 0)
		return (ERROR);
	while (cbuf_len(b) > 0)
	{
		debug_search_start(cbuf_len(b), ryker_ft_max(1, depth));
		if (choose_plan(a, b, depth, &best) == ERROR)
			return (ERROR);
		debug_greedy_execute(&best);
		if (greedy_execute_plan(x, a, b, &best) == ERROR)
			return (ERROR);
	}
	return (SUCCESS);
}

int	greedy_reinsertion(soln *x, circle_buf *a, circle_buf *b,
	t_algorithm algo)
{
	const t_algo_config	*config;

	config = algorithm_config(algo);
	if (!config || greedy_prepare(x, a, b, config->seed) == ERROR)
		return (ERROR);
	debug_pass_start(cbuf_len(b), ryker_ft_max(1, config->depth), algo);
	if (greedy_insert_all(x, a, b, config->depth) == ERROR)
		return (ERROR);
	return (rot_a_min_to_top(x, a));
}
