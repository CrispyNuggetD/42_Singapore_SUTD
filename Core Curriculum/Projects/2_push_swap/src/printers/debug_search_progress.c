/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_search_progress.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 06:01:57 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 18:43:55 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"
#include <limits.h>

/* One synchronous depth-first search is active at a time. */
t_search_debug	*debug_search_state(void)
{
	static t_search_debug	state;

	return (&state);
}

/* Sum P(b, 1) + ... + P(b, min(depth, b)); saturate instead of wrapping. */
void	debug_count_trials(t_search_debug *s)
{
	unsigned long long	paths;
	int					level;

	paths = 1;
	level = 0;
	while (level < s->depth_limit)
	{
		if (paths > ULLONG_MAX / (s->initial_b - level))
			s->capped = 1;
		if (!s->capped)
			paths *= s->initial_b - level;
		if (s->capped || s->total > ULLONG_MAX - paths)
		{
			s->capped = 1;
			s->total = ULLONG_MAX;
			return ;
		}
		s->total += paths;
		level++;
	}
}

void	debug_search_start(int b_len, int depth)
{
	t_search_debug	*s;

	if (!DEBUG)
		return ;
	s = debug_search_state();
	debug_search_reset(s, b_len, depth);
	if (depth == 0 || DEBUG == 1)
		return ;
	ryker_ft_printf_fd(2, "[lookahead] START candidates=%d depth=%d",
		b_len, s->depth_limit);
	ryker_ft_printf_fd(2, " requested=%d trials=%llu", depth, s->total);
	if (s->capped)
		ryker_ft_printf_fd(2, "+ (counter limit)");
	ryker_ft_printf_fd(2, "\n");
}

void	debug_search_prefix(const char *event, int depth)
{
	t_search_debug	*s;
	int				level;

	if (!DEBUG)
		return ;
	s = debug_search_state();
	level = s->root_depth - depth;
	ryker_ft_printf_fd(2, "[lookahead depth=%d/%d candidate=%d/%d] %s",
		level + 1, s->depth_limit, s->candidate[level],
		s->initial_b - level, event);
}

void	debug_search_progress(void)
{
	t_search_debug	*s;

	if (!DEBUG)
		return ;
	s = debug_search_state();
	if (s->capped)
		ryker_ft_printf_fd(2, " completed=%llu total=%llu+", s->done, s->total);
	else
		ryker_ft_printf_fd(2, " completed=%llu/%llu remaining=%llu",
			s->done, s->total, s->total - s->done);
}
