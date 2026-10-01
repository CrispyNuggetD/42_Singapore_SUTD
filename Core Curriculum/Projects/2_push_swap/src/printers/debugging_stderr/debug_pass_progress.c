/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_pass_progress.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 06:01:57 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 18:43:55 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"
#include <limits.h>

/* Add only a search that actually starts; future batch lengths are unknown. */
static void	count_pass(t_search_debug *s)
{
	if (s->capped || s->pass_total > ULLONG_MAX - s->total)
	{
		s->pass_capped = 1;
		s->pass_total = ULLONG_MAX;
	}
	else
		s->pass_total += s->total;
	s->skipped = 0;
}

void	debug_pass_start(int b_len, int depth, int algo)
{
	t_search_debug		*s;
	const t_algo_config	*config;

	if (!DEBUG)
		return ;
	s = debug_search_state();
	ft_memset(s, 0, sizeof(*s));
	s->algo_id = algo + 1;
	s->pass_initial_b = b_len;
	s->printed_tenths = -1;
	s->algo_label = " (Greedy reinsertion)";
	config = algorithm_config(algo);
	if (config)
		s->algo_label = config->name;
	s->pass_active = 1;
	s->root_depth = depth;
	debug_status_prepare(s);
}

/* Search counts reset; the insertion percentage spans the whole pass. */
void	debug_search_reset(t_search_debug *s, int b_len, int depth)
{
	if (!DEBUG)
		return ;
	if (!s->pass_active)
		debug_pass_start(b_len, depth, -1);
	s->root_depth = depth;
	s->depth_limit = depth;
	ryker_ft_update_min(&s->depth_limit, b_len);
	s->initial_b = b_len;
	s->active = 1;
	s->done = 0;
	s->total = 0;
	s->capped = 0;
	s->best_index = 0;
	s->roots_done = 0;
	s->evaluated = 0;
	s->status_level = 0;
	s->best_total = 0;
	debug_count_trials(s);
	count_pass(s);
}

/* Insertion-based throttling; trial counts never advance the percentage. */
int	debug_status_ready(t_search_debug *s, int complete)
{
	if (DEBUG != 1)
		return (0);
	while (s->pass_initial_b > 0 && s->percent_tenths < 1000
		&& (unsigned long long)s->inserted
		>= s->threshold[s->percent_tenths + 1])
		s->percent_tenths++;
	if (complete)
		return (1);
	if (s->printed_tenths == s->percent_tenths)
		return (0);
	s->printed_tenths = s->percent_tenths;
	return (1);
}

/* Called only after a successful insertion on the real stacks. */
void	debug_insertion_done(void)
{
	t_search_debug	*s;

	if (!DEBUG)
		return ;
	s = debug_search_state();
	s->inserted++;
	s->status_level = 0;
	debug_status_draw(2);
}
