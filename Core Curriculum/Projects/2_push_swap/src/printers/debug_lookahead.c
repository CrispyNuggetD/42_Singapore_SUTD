/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_lookahead.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 06:01:57 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 18:43:55 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"
#include <limits.h>

void	debug_lookahead_try(int depth, int b_len, const t_greedy_plan *plan)
{
	t_search_debug	*s;
	int				index;

	if (!DEBUG)
		return ;
	s = debug_search_state();
	if (!s->active)
		debug_search_start(b_len, depth);
	index = plan->rot_b;
	if (index < 0)
		index += b_len;
	s->candidate[s->root_depth - depth] = index + 1;
	if (DEBUG == 1 || (DEBUG < 3 && depth != s->root_depth))
		return ;
	debug_search_prefix("TRY", depth);
	debug_search_progress();
	if (DEBUG >= 4)
		ryker_ft_printf_fd(2, " rank=%d immediate=%d",
			plan->candidate_rank, plan->cost);
	ryker_ft_printf_fd(2, "\n");
}

void	debug_lookahead_stop(int depth, int cost, const char *reason)
{
	if (DEBUG < 3)
		return ;
	if (debug_search_state()->active)
		debug_search_prefix("STOP", depth + 1);
	else
		ryker_ft_printf_fd(2, "[lookahead] STOP");
	ryker_ft_printf_fd(2, " %s", reason);
	if (DEBUG >= 4)
		ryker_ft_printf_fd(2, " cost=%d", cost);
	ryker_ft_printf_fd(2, "\n");
}

void	debug_greedy_execute(const t_greedy_plan *plan)
{
	t_search_debug	*s;
	int				index;

	if (DEBUG < 2)
		return ;
	s = debug_search_state();
	index = plan->rot_b;
	if (index < 0)
		index += s->initial_b;
	ryker_ft_printf_fd(2, "[greedy] EXECUTE candidate=%d/%d B_remaining=%d",
		index + 1, s->initial_b, s->initial_b - 1);
	if (DEBUG >= 4)
		ryker_ft_printf_fd(2, " rank=%d cost=%d",
			plan->candidate_rank, plan->cost);
	ryker_ft_printf_fd(2, "\n");
}

void	debug_lis_length(int length)
{
	if (DEBUG < 2)
		return ;
	ryker_ft_printf_fd(2, "best_circular_lis_len: %i\n", length);
}
