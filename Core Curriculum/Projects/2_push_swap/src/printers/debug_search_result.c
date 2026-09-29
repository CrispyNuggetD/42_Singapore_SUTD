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

/* ceil(total * percent / 100), without overflowing total * percent. */
void	debug_status_prepare(t_search_debug *s)
{
	unsigned long long	base;
	unsigned long long	whole;
	int					remainder;
	int					carry;
	int					i;

	if (DEBUG != 1 || s->pass_capped)
		return ;
	base = s->pass_total / 100;
	remainder = s->pass_total % 100;
	whole = 0;
	carry = 0;
	i = 0;
	while (++i <= 100)
	{
		whole += base;
		carry += remainder;
		if (carry >= 100)
		{
			whole++;
			carry -= 100;
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
	debug_search_prefix("RETURN", depth);
	debug_search_progress();
	if (DEBUG >= 4)
		ryker_ft_printf_fd(2, " immediate=%d remaining_cost=%d total_cost=%d",
			plan->cost, score - plan->cost, score);
	if (best_score < 0 || score < best_score)
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
	improved = (best_score < 0 || score < best_score);
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

/* Keep one line across real insertions; end it after the final insertion. */
void	debug_status_end(void)
{
	t_search_debug	*s;

	if (DEBUG != 1)
		return ;
	s = debug_search_state();
	if (s->initial_b == 1 && s->root_depth > 0)
	{
		s->pass_active = 0;
		ryker_ft_printf_fd(2, "\n");
	}
}
