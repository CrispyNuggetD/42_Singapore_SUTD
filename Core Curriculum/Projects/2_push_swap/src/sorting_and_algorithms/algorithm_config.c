/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   algorithm_config.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 19:50:04 by hnah              #+#    #+#             */
/*   Updated: 2026/09/30 20:15:05 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

/* Validate the configured search depths and execution limits together. */
static int	validate_settings(void)
{
	if (EXECUTE_LIMIT_100 < 1 || EXECUTE_LIMIT_100 > LOOKAHEAD_DEPTH_100
		|| EXECUTE_LIMIT_500 < 1 || EXECUTE_LIMIT_500 > LOOKAHEAD_DEPTH_500
		|| EXECUTE_LIMIT_FIRST_MOVE < 1
		|| EXECUTE_LIMIT_FIRST_MOVE > LOOKAHEAD_DEPTH_FIRST_MOVE
		|| LOOKAHEAD_DEPTH_100 > GREEDY_PATH_CAPACITY
		|| LOOKAHEAD_DEPTH_500 > GREEDY_PATH_CAPACITY
		|| LOOKAHEAD_DEPTH_FIRST_MOVE > GREEDY_PATH_CAPACITY)
		return (ERROR);
	return (SUCCESS);
}

/* One table drives execution and names; callers never infer seed from IDs. */
const t_algo_config	*algorithm_config(t_algorithm algo)
{
	static const t_algo_config	configs[ALGO_COUNT] = {
	{SEED_COUNT, 0, " (BFS Brute-Force Optimum Moves)"},
	{SEED_THREE, 0, " (3-element seed + local greedy)"},
	{SEED_LIS, 0, " (Circular LIS + local greedy)"},
	{SEED_LIS, 1, " (Circular LIS + lookahead greedy)"},
	{SEED_LIS, 1, " (Circular LIS + opening only 1 insertion "
		"with extra lookahead greedy)"},
	{SEED_LIS, 1, " (Circular LIS + opening batch moves "
		"from extra lookahead greedy)"},
		/* {SEED_THREE, 1, " (3-element seed + lookahead)"}, */
	};

	if (algo < ALGO_BFS || algo >= ALGO_COUNT
		|| (algo != ALGO_BFS && validate_settings() == ERROR))
		return (NULL);
	return (&configs[algo]);
}
