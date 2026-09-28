/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stack_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:42:07 by hnah              #+#    #+#             */
/*   Updated: 2026/09/28 23:17:53 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

// 20260723(Thu)13:34:11+08:00

void	append_move_to_soln(soln *x, char move)
{
	x->ans[x->cur][x->step] = move;
	x->step++;
	x->ans_len[x->cur] = x->step;
}

int	soln_init(soln *x, const int soln_num, const int steps_limit)
{
	int	cur_soln;

	x->cur = -1;
	x->step = -1;
	x->ans_len = malloc(sizeof(int) * soln_num);
	x->ans = malloc(sizeof(char *) * soln_num);
	if (!x->ans)
		return (ERROR);
	cur_soln = (int)soln_num;
	while (cur_soln-- > 0)
	{
		x->ans[cur_soln] = malloc(sizeof(char) * steps_limit);
		if (!x->ans[cur_soln])
		{
			while (++cur_soln < soln_num)
				free(x->ans[cur_soln]);
			free(x->ans);
			return (ERROR);
		}
	}
	return (SUCCESS);
}

int	new_soln_init(soln *x, circle_buf stacks[2], circle_buf *a_ori, circle_buf *b_ori)
{
	if (!x || !stacks || !a_ori || !b_ori)
		return (ERROR);
	x->cur++;
	x->step = 0;
	stacks[A] = *a_ori;
	stacks[B] = *b_ori;
	return (SUCCESS);
}
/*
int	larger_top(soln *x, circle_buf *a)
{
	int	top_idx;

	top_idx = a->read_idx;
	return (a->buf[top_idx] > a->buf[top_idx + 1]);
}
*/

int	get_order_top_three(circle_buf *a)
{
	int	first_idx;
	int	second_idx;
	int	third_idx;

	first_idx = a->read_idx;
	second_idx = (a->read_idx + 1) % a->capacity;
	third_idx = (a->read_idx + 2) % a->capacity;
	if (a->buf[first_idx] > a->buf[second_idx])
	{
		if (a->buf[second_idx] > a->buf[third_idx])
			return (321);
		else if (a->buf[third_idx] > a->buf[first_idx])
			return (213);
		else
			return (312);
	}
	else
	{
		if (a->buf[third_idx] > a->buf[second_idx])
			return (123);
		else if (a->buf[first_idx] > a->buf[third_idx])
			return (231);
	}
	return (132);
}

/*
** Bring A's minimum to its top by the cheaper rotation direction.
** After update_min, top_rank represents remaining moves
** Requires circularly ascending A + All ranks present. Empty/singleton/already aligned: SUCCESS.
*/
int	rot_a_min_to_top(soln *x, circle_buf *a)
{
	int	top_rank;
	int	outcome;
	int	direction;

	direction = 1;
	if (cbuf_len(a) == 0)
		return (SUCCESS);
	if (cbuf_read_at(a, 0, &top_rank) != SUCCESS)
		return (ERROR);
	if (ryker_ft_update_min(&top_rank, cbuf_opp_moves(a, top_rank)) == 0)
		direction = -1;
	outcome = SUCCESS;
	while (top_rank != 0 && outcome == SUCCESS)
	{
		if (direction == -1)
			outcome = rra(x, a);
		else
			outcome = ra(x, a);
		if (outcome == SUCCESS)
			top_rank--;
	}
	return (outcome);
}
