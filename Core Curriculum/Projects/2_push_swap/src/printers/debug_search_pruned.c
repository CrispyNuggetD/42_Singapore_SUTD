/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_search_pruned.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 06:01:57 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 18:43:55 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"
#include <limits.h>

static void	add_saturated(unsigned long long *value,
	unsigned long long extra)
{
	if (*value > ULLONG_MAX - extra)
		*value = ULLONG_MAX;
	else
		*value += extra;
}

/* Count descendants only: the rejected candidate itself was evaluated. */
static unsigned long long	skipped_trials(int depth, int b_len)
{
	unsigned long long	paths;
	unsigned long long	total;

	paths = 1;
	total = 0;
	while (--depth > 0 && --b_len > 0)
	{
		if (paths > ULLONG_MAX / b_len)
			return (ULLONG_MAX);
		paths *= b_len;
		add_saturated(&total, paths);
	}
	return (total);
}

/* Cover the discarded subtree without simulating or reporting each node. */
void	debug_lookahead_pruned(int depth, int b_len)
{
	t_search_debug		*s;
	unsigned long long	skipped;

	if (!DEBUG)
		return ;
	s = debug_search_state();
	skipped = skipped_trials(depth, b_len);
	add_saturated(&s->done, skipped);
	add_saturated(&s->pass_done, skipped);
	add_saturated(&s->pass_skipped, skipped);
}
