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
# define GREEDY_PRUNED -2

/*
** One B -> A transfer, valid only for the state used to calculate it.
** Positive rotations mean forward; negative mean reverse; zero means none.
** cost counts printed operations, including the final pa.
** Plan builders return SUCCESS; search selectors return a nonnegative cost.
*/
typedef struct s_greedy_plan
{
	int	candidate_rank;
	int	rot_a;
	int	rot_b;
	int	cost;
}	t_greedy_plan;

/* Only plans[0..length-1] are valid, in execution order. */
typedef struct s_greedy_path
{
	t_greedy_plan	plans[GREEDY_PATH_CAPACITY];
	int				length;
}	t_greedy_path;

/* Passed by value: each call owns its depth and exclusive cost budget. */
typedef struct s_greedy_search
{
	int				depth;
	int				budget;
	t_greedy_path	*best_path;
}	t_greedy_search;

/* Debug printers return immediately when the header's DEBUG flag is zero. */
void	debug_lookahead_try(int depth, int b_len,
			const t_greedy_plan *plan);
void	debug_lookahead_result(int depth, const t_greedy_plan *plan,
			int score, int best_score);
void	debug_lookahead_stop(int depth, int cost, const char *reason);
void	debug_greedy_execute(const t_greedy_plan *plan);

/* Scan returns a raw position; greedy_find_target normalises it. */
int		greedy_scan_target(t_circle_buf *a, int desired_rank, int scan_dir);
/* Remaining functions return SUCCESS / ERROR; outputs require SUCCESS. */
int		greedy_find_target(t_circle_buf *a, int rank, int *target_index);
int		greedy_plan_candidate(t_circle_buf *a, t_circle_buf *b, int b_index,
			t_greedy_plan *plan);
int		greedy_choose_plan_local(t_circle_buf *a, t_circle_buf *b,
			t_greedy_plan *best_first_plan);
/*
** Requires valid stacks, circularly ascending A, and all ranks 0..n-1
** distributed across A and B, each exactly once.
** Searches return cost >= 0, -1 on error, or GREEDY_PRUNED if over budget.
** Inputs are unchanged. Pruning preserves the first minimum in this horizon.
** Empty B costs final alignment; otherwise depth zero costs zero.
** choose requires depth >= 1 and nonempty B. First minimum wins ties.
** best_path->plans[0].cost is immediate; return value is the search cost.
** Saves the winning path, capped by the search depth and remaining B.
*/
/* A pruned search leaves the output path untouched. Budget is exclusive. */
int		greedy_lookahead_cost(t_circle_buf *a, t_circle_buf *b,
			t_greedy_search search);
int		greedy_choose_bounded(t_circle_buf *a, t_circle_buf *b,
			t_greedy_search search);
int		greedy_branch_cost(t_circle_buf *a, t_circle_buf *b,
			t_greedy_search search, t_greedy_path *candidate_path);
int		greedy_choose_plan_lookahead(t_circle_buf *a, t_circle_buf *b,
			int depth, t_greedy_path *best_path);
int		greedy_execute_plan(t_soln *x, t_circle_buf *a, t_circle_buf *b,
			const t_greedy_plan *plan);

/* Requires empty or circularly ascending A; accepts arbitrary B. */
int		greedy_insert_all(t_soln *x, t_circle_buf stacks[2], int use_lookahead);
int		greedy_prepare(t_soln *x, t_circle_buf *a, t_circle_buf *b,
			t_seed_mode mode);
#endif
