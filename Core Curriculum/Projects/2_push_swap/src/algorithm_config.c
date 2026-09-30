/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   algorithm_config.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 19:50:04 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 19:54:32 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

/* One table drives execution and names; callers never infer seed from IDs. */
const t_algo_config	*algorithm_config(t_algorithm algo)
{
	static const t_algo_config	configs[ALGO_COUNT] = {
	{SEED_LIS, 0, " (Circular LIS + local greedy)"},
	{SEED_THREE, 0, " (3-element seed + local greedy)"},
	{SEED_LIS, LOOKAHEAD_DEPTH, " (Circular LIS + lookahead)"},
		/* {SEED_THREE, LOOKAHEAD_DEPTH, " (3-element seed + lookahead)"}, */
	};

	if (algo < ALGO_LIS_LOCAL || algo >= ALGO_COUNT)
		return (NULL);
	if (algo >= ALGO_LIS_LOOKAHEAD && LOOKAHEAD_DEPTH < 1)
		return (NULL);
	return (&configs[algo]);
}
