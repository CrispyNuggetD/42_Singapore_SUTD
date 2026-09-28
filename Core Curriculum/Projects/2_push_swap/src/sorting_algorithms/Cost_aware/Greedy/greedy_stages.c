/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   greedy_stages.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 18:03:22 by hnah              #+#    #+#             */
/*   Updated: 2026/09/28 22:49:12 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"

/*
** TODO: Baseline preparation starts with empty B; leave at most three in A
** and sort that seed. Handle zero, one and two elements too.
** Postcondition: A is empty or circularly ascending; all ranks are retained.
** Keep this independent of insertion, so a future LIS/BFS seed can replace it.
** Existing small-sort helper is static in solve.c; inspect it before reuse.
*/
int	greedy_prepare(soln *x, circle_buf *a, circle_buf *b)
{
	int	total_len;
	int	seed_len;
	int	push_to_b;
	int	outcome;

	seed_len = 3;
	total_len = cbuf_len(a);
	push_to_b = total_len - seed_len;
	outcome = SUCCESS;
	while (push_to_b > 0 && outcome == SUCCESS)
	{
		outcome = pb(x, a, b);
		push_to_b--;
	}
	if (outcome == SUCCESS)
		outcome = hardcode_three(x, a);
	return (outcome);
} 

static int	greedy_prepare_LIS(soln *x, circle_buf *a, circle_buf *b,
	char keep_flags[500])
{
	int	total_len;
	int	outcome;
	int	i;

	total_len = cbuf_len(a);
	outcome = SUCCESS;
	i = 0;
	while (total_len > 0 && outcome == SUCCESS)
	{
		if (keep_flags[i] == 0)
			outcome = pb(x, a, b);
		else if (keep_flags[i] == 1)
			outcome = ra(x, a);
		total_len--;
		i++;
	}
	return (outcome);
}


/*
** TODO: Repeatedly choose and execute a fresh plan until B is empty.
** Already empty B is SUCCESS. Do not prepare A here or reuse stale plans.
** Caller supplies empty or circularly ascending A, not a hidden unsorted tail.
*/
int	greedy_insert_all(soln *x, circle_buf *a, circle_buf *b)
{
	t_greedy_plan	best_plan;

	while (cbuf_len(b) && greedy_choose_plan(a, b, &best_plan) == SUCCESS)
	{
		if (greedy_execute_plan(x, a, b, &best_plan) == ERROR)
			return (ERROR);
	}
	if (cbuf_len(b) == 0)
		return (SUCCESS);
	return (ERROR);
}

static int	greedy_non_shared_moves(soln *x, circle_buf *a, circle_buf *b,
		t_greedy_plan *temp)
{
	while (temp->rot_a > 0)
	{
		if (ra(x, a) != SUCCESS)
			return (ERROR);
		temp->rot_a--;
	}
	while (temp->rot_a < 0)
	{
		if (rra(x, a) != SUCCESS)
			return (ERROR);
		temp->rot_a++;
	}
	while (temp->rot_b > 0)
	{
		if (rb(x, b) != SUCCESS)
			return (ERROR);
		temp->rot_b--;
	}
	while (temp->rot_b < 0)
	{
		if (rrb(x, b) != SUCCESS)
			return (ERROR);
		temp->rot_b++;
	}
	return (SUCCESS);
}

/*
** TODO: Execute a fresh plan using existing operation wrappers and soln.
** Share compatible rotations, finish leftovers, then push. Check failures.
** Use local remaining counts so the caller's plan stays unchanged.
*/
int	greedy_execute_plan(soln *x, circle_buf *a, circle_buf *b,
		const t_greedy_plan *plan)
{
	t_greedy_plan	temp;
	int				shared_rot;
	int				outcome;

	temp = *plan;
	outcome = 0;
	shared_rot = (temp.rot_a * temp.rot_b) > 0;
	shared_rot = shared_rot * ryker_ft_sign(temp.rot_a);
	while (shared_rot != 0 && temp.rot_a != 0
		&& temp.rot_b != 0 && outcome == 0)
	{
		if (shared_rot == 1)
			outcome = rr(x, a, b);
		if (shared_rot == -1)
			outcome = rrr(x, a, b);
		if (outcome == 0)
		{
			temp.rot_a -= shared_rot;
			temp.rot_b -= shared_rot;
		}
	}
	if (outcome != 0 || greedy_non_shared_moves(x, a, b, &temp) == ERROR)
		return (ERROR);
	return (pa(x, a, b));
}

/*
** Compose preparation, insertion and alignment; propagate failures.
** Initial contract: B empty, distinct normalised ranks stored in A.
** After implementation, select this in solve.c when ready to try it.
** Future seed strategies can instead call
** insert_all and rot_a_min_to_top directly.
*/
int	greedy_reinsertion(soln *x, circle_buf *a, circle_buf *b, char keep_flags[500])
{
	if (!keep_flags && (greedy_prepare(x, a, b) == ERROR))
		return (ERROR);
	else if (keep_flags && (greedy_prepare_LIS(x, a, b, keep_flags) == ERROR))
		return (ERROR);
	if (greedy_insert_all(x, a, b) || rot_a_min_to_top(x, a) == ERROR)
		return (ERROR);
	return (SUCCESS);
}
