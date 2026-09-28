/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   greedy_execute.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:11:30 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 01:23:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"

static int	rotate_a(soln *x, circle_buf *a, int count)
{
	int	outcome;

	while (count != 0)
	{
		if (count > 0)
			outcome = ra(x, a);
		else
			outcome = rra(x, a);
		if (outcome == ERROR)
			return (ERROR);
		count -= ryker_ft_sign(count);
	}
	return (SUCCESS);
}

static int	rotate_b(soln *x, circle_buf *b, int count)
{
	int	outcome;

	while (count != 0)
	{
		if (count > 0)
			outcome = rb(x, b);
		else
			outcome = rrb(x, b);
		if (outcome == ERROR)
			return (ERROR);
		count -= ryker_ft_sign(count);
	}
	return (SUCCESS);
}

static int	shared_moves(soln *x, circle_buf *a, circle_buf *b,
	t_greedy_plan *plan)
{
	int	direction;
	int	outcome;

	while (plan->rot_a * plan->rot_b > 0)
	{
		direction = ryker_ft_sign(plan->rot_a);
		if (direction > 0)
			outcome = rr(x, a, b);
		else
			outcome = rrr(x, a, b);
		if (outcome == ERROR)
			return (ERROR);
		plan->rot_a -= direction;
		plan->rot_b -= direction;
	}
	return (SUCCESS);
}

int	greedy_execute_plan(soln *x, circle_buf *a, circle_buf *b,
	const t_greedy_plan *plan)
{
	t_greedy_plan	remaining;

	remaining = *plan;
	if (shared_moves(x, a, b, &remaining) == ERROR)
		return (ERROR);
	if (rotate_a(x, a, remaining.rot_a) == ERROR
		|| rotate_b(x, b, remaining.rot_b) == ERROR)
		return (ERROR);
	return (pa(x, a, b));
}
