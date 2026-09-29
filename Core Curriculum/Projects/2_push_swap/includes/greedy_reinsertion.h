/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   greedy_reinsertion.h                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 18:52:39 by hnah              #+#    #+#             */
/*   Updated: 2026/09/23 18:05:28 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GREEDY_REINSERTION_H
# define GREEDY_REINSERTION_H

# include "push_swap.h"
# include "debug_search.h"

/* Internal greedy planning API; the solver entry point is in push_swap.h. */
/* Logical positions count from the top, never from buf[0]. */
# define GREEDY_NO_TARGET -1

/*
** One B -> A transfer, valid only for the state used to calculate it.
** Positive rotations mean forward; negative mean reverse; zero means none.
** cost counts printed operations, including the final pa.
** Only use an output plan when its producer returns SUCCESS.
*/
typedef struct s_greedy_plan
{
	int	candidate_rank;
	int	rot_a;
	int	rot_b;
	int	cost;
}	t_greedy_plan;

/* Debug printers return immediately when the header's DEBUG flag is zero. */
void	debug_lookahead_try(int depth, int b_len,
			const t_greedy_plan *plan);
void	debug_lookahead_result(int depth, const t_greedy_plan *plan,
			int score, int best_score);
void	debug_lookahead_stop(int depth, int cost, const char *reason);
void	debug_greedy_execute(const t_greedy_plan *plan);

/* Scan returns a raw position; greedy_find_target normalises it. */
int		greedy_scan_target(circle_buf *a, int desired_rank, int scan_dir);
/* Remaining functions return SUCCESS / ERROR; outputs require SUCCESS. */
int		greedy_find_target(circle_buf *a, int rank, int *target_index);
int		greedy_plan_candidate(circle_buf *a, circle_buf *b, int b_index,
			t_greedy_plan *plan);
int		greedy_choose_plan_local(circle_buf *a, circle_buf *b,
			t_greedy_plan *best_first_plan);
/*
** Requires valid stacks, circularly ascending A, and all ranks 0..n-1
** distributed across A and B, each exactly once.
** Both functions return a nonnegative search cost, or -1 on error.
** Inputs are unchanged. Every B candidate is explored: use small depths.
** Empty B costs final alignment; otherwise depth zero costs zero.
** choose requires depth >= 1 and nonempty B. First minimum wins ties.
** best_first_plan->cost is immediate; the return value is the search cost.
*/
int		greedy_lookahead_cost(circle_buf *a, circle_buf *b, int depth);
int		greedy_choose_plan_lookahead(circle_buf *a, circle_buf *b, int depth,
			t_greedy_plan *best_first_plan);
int		greedy_execute_plan(soln *x, circle_buf *a, circle_buf *b,
			const t_greedy_plan *plan);

/* Requires empty or circularly ascending A; accepts arbitrary B. */
int		greedy_insert_all(soln *x, circle_buf *a, circle_buf *b, int depth);
int		greedy_prepare(soln *x, circle_buf *a, circle_buf *b,
			t_seed_mode mode);
#endif
