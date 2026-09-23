#ifndef GREEDY_REINSERTION_H
# define GREEDY_REINSERTION_H

# include "push_swap.h"

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

/* All functions return SUCCESS / ERROR. Outputs are valid on SUCCESS only. */
int	greedy_find_target(circle_buf *a, int rank, int *target_index);
int	greedy_plan_candidate(circle_buf *a, circle_buf *b, int b_index,
		t_greedy_plan *plan);
int	greedy_choose_plan(circle_buf *a, circle_buf *b, t_greedy_plan *best);
int	greedy_execute_plan(soln *x, circle_buf *a, circle_buf *b,
		const t_greedy_plan *plan);

/* Preparation is replaceable: three-element seed now, LIS/BFS later. */
int	greedy_prepare(soln *x, circle_buf *a, circle_buf *b);
/* Requires empty or circularly ascending A; accepts arbitrary B. */
int	greedy_insert_all(soln *x, circle_buf *a, circle_buf *b);
int	greedy_align_min(soln *x, circle_buf *a);
/* Full baseline: prepare, insert all, align minimum. */
int	greedy_reinsertion(soln *x, circle_buf *a, circle_buf *b);

#endif
