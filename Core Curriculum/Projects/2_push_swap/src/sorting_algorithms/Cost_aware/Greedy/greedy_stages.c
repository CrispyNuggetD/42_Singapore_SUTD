/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   greedy_stages.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:11:30 by hnah              #+#    #+#             */
/*   Updated: 2026/09/30 15:33:13 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"

/* Exactly one selector writes the plan; normalise its return convention. */
static int	choose_plan(circle_buf *a, circle_buf *b, int depth,
	t_greedy_path *best)
{
	t_greedy_plan	first_move[500];
	int				first_elem;
	int				b_length;

	if (depth == 0)
	{
		best->length = 1;
		return (greedy_choose_plan_local(a, b, &best->plans[0]));
	}
	b_length = cbuf_len(b);
	first_elem = -1;
	while (++first_elem < b_length)
	{
		if (greedy_plan_candidate(a, b, first_elem,
			&first_move[first_elem]) == ERROR)
		return (ERROR);
	}
	
	if (greedy_choose_plan_lookahead(a, b, depth, best) < 0)
		return (ERROR);
	return (SUCCESS);
}

/* Preparation leaves A circularly ascending; insertion preserves that. */
int	greedy_insert_all(soln *x, circle_buf *a, circle_buf *b, int depth)
{
	t_greedy_path	best;
	int				i;

	if (depth < 0 || EXECUTE_DEPTH < 1 || EXECUTE_DEPTH > LOOKAHEAD_DEPTH)
		return (ERROR);
	while (cbuf_len(b) > 0)
	{
		debug_search_start(cbuf_len(b), ryker_ft_max(1, depth));
		if (choose_plan(a, b, depth, &best) == ERROR)
			return (ERROR);
		i = 0;
		while (i < ryker_ft_min(EXECUTE_DEPTH, best.length))
		{
			debug_greedy_execute(&best.plans[i]);
			if (greedy_execute_plan(x, a, b, &best.plans[i]) == ERROR)
				return (ERROR);
			debug_insertion_done();
			i++;
		}
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
	if (LOOKAHEAD_DEPTH >= 1
		&& greedy_insert_all(x, a, b, config->depth) == ERROR)
		return (ERROR);
	if (rot_a_min_to_top(x, a) == ERROR)
		return (ERROR);
	debug_status_end(x->step);
	return (SUCCESS);
}
