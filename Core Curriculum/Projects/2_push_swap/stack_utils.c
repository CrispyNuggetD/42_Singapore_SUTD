/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   circular_buffer_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:42:07 by hnah              #+#    #+#             */
/*   Updated: 2026/04/23 13:46:11 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

// 20260723(Thu)13:34:11+08:00

void	append(soln *x, char move)
{
	x->ans[x->cur][x->step] = move;
	x->step++;
	// x->ans_len[x->cur] = x->step;
}

int	soln_init(soln *x, const int soln_num, const int steps_limit)
{
	int	cur_soln;

	x->cur = 0;
	x->step = 0;
	x->ans_len = malloc(sizeof(int) * soln_num);
	x->ans = malloc(sizeof(char *)	 * soln_num);
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

int	larger_top(soln *x, cbuf *a)
{
	int	top_idx;

	top_idx = a->read_idx;
	return (a->buf[top_idx] > a->buf[top_idx + 1]);
}

int get_order_top_three(soln *x, cbuf *a)
{
	int	first_idx;
	int	second_idx;
	int	third_idx;

	first_idx = a->read_idx;
	second_idx = (a->read_idx + 1) % a->capacity;
	third_idx = (a->read_idx + 2) % a->capacity;
	if (a->buf[top_idx] > a->buf[second_idx])
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
