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

/* Scan returns a raw position; greedy_find_target normalises it. */
int	greedy_scan_target(circle_buf *a, int desired_rank, int scan_dir);
/* Remaining functions return SUCCESS / ERROR; outputs require SUCCESS. */
int	greedy_find_target(circle_buf *a, int rank, int *target_index);
int	greedy_plan_candidate(circle_buf *a, circle_buf *b, int b_index,
		t_greedy_plan *plan);
int	greedy_choose_plan(circle_buf *a, circle_buf *b, t_greedy_plan *best);
int	greedy_execute_plan(soln *x, circle_buf *a, circle_buf *b,
		const t_greedy_plan *plan);

/* Requires empty or circularly ascending A; accepts arbitrary B. */
int	greedy_insert_all(soln *x, circle_buf *a, circle_buf *b);
int	greedy_prepare(soln *x, circle_buf *a, circle_buf *b);
#endif
