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
	{SEED_THREE, 0, " (3-element seed + local greedy)"},
	{SEED_LIS, 0, " (Circular LIS + local greedy)"},
	{SEED_THREE, LOOKAHEAD, " (3-element seed + lookahead)"},
	{SEED_LIS, LOOKAHEAD, " (Circular LIS + lookahead)"}};

	if (algo < ALGO_THREE_LOCAL || algo >= ALGO_COUNT)
		return (NULL);
	if (algo >= ALGO_THREE_LOOKAHEAD && LOOKAHEAD < 1)
		return (NULL);
	return (&configs[algo]);
}
