/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_search_result.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 06:01:57 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 18:43:55 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"
#include <limits.h>

/* Integer thresholds for 0.1% steps, without overflowing total * step. */
void	debug_status_prepare(t_search_debug *s)
{
	unsigned long long	base;
	unsigned long long	whole;
	int					remainder;
	int					carry;
	int					i;

	if (DEBUG != 1 || s->pass_capped)
		return ;
	base = s->pass_total / 1000;
	remainder = s->pass_total % 1000;
	whole = 0;
	carry = 0;
	i = 0;
	while (++i <= 1000)
	{
		whole += base;
		carry += remainder;
		if (carry >= 1000)
		{
			whole++;
			carry -= 1000;
		}
		s->threshold[i] = whole + (carry != 0);
	}
}

static void	result_details(int depth, const t_greedy_plan *plan,
	int score, int best_score)
{
	if (DEBUG < 2)
		return ;
	if (DEBUG < 3 && depth != debug_search_state()->root_depth)
		return ;
	if (score == GREEDY_PRUNED)
		debug_search_prefix("PRUNED", depth);
	else
		debug_search_prefix("RETURN", depth);
	debug_search_progress();
	if (DEBUG >= 4 && score >= 0)
		ryker_ft_printf_fd(2, " immediate=%d remaining_cost=%d total_cost=%d",
			plan->cost, score - plan->cost, score);
	if (score >= 0 && (best_score < 0 || score < best_score))
		ryker_ft_printf_fd(2, " -> BEST FIRST PLAN");
	ryker_ft_printf_fd(2, "\n");
}

/* Child progress may redraw; only root results replace the saved winner. */
static void	root_status(int depth, int score, int best_score, int complete)
{
	t_search_debug	*s;
	int				improved;

	if (DEBUG != 1)
		return ;
	s = debug_search_state();
	improved = (score >= 0 && (best_score < 0 || score < best_score));
	if (depth == s->root_depth && improved)
	{
		s->best_index = s->candidate[0];
		s->best_total = score;
	}
	if (s->best_index > 0 && (improved || complete))
	{
		s->status_level = s->root_depth - depth;
		debug_status_draw(complete);
	}
}

void	debug_lookahead_result(int depth, const t_greedy_plan *plan,
	int score, int best_score)
{
	t_search_debug	*s;
	int				complete;

	if (!DEBUG)
		return ;
	s = debug_search_state();
	if (s->done < ULLONG_MAX)
		s->done++;
	if (s->pass_done < ULLONG_MAX)
		s->pass_done++;
	complete = (depth == s->root_depth
			&& s->candidate[0] == s->initial_b);
	result_details(depth, plan, score, best_score);
	root_status(depth, score, best_score, complete);
	if (complete)
		s->active = 0;
}

/* Finish after alignment, including preparation and all recorded moves. */
void	debug_status_end(int moves)
{
	t_search_debug	*s;

	if (!DEBUG)
		return ;
	s = debug_search_state();
	if (DEBUG == 1)
		ryker_ft_printf_fd(2, "\r\033[2K");
	ryker_ft_printf_fd(2, "covered=%llu/%llu skipped=%llu [##########] 100.0%%",
		s->pass_done, s->pass_total, s->pass_skipped);
	ryker_ft_printf_fd(2, " inserted=%d/%d", s->inserted, s->pass_initial_b);
	ryker_ft_printf_fd(2, " algo=%d/%d%s final_moves=%d DONE\n",
		s->algo_id, ALGO_COUNT, s->algo_label, moves);
	s->pass_active = 0;
}
