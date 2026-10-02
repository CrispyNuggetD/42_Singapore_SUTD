/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   solution_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:42:07 by hnah              #+#    #+#             */
/*   Updated: 2026/10/01 18:47:46 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

/* Only the active answer grows; old candidate buffers are never reused. */
static int	grow_answer(t_soln *x)
{
	char	*answer;
	int		capacity;

	if (x->capacity == INT_MAX)
		return (ERROR);
	capacity = INT_MAX;
	if (x->capacity <= INT_MAX / 2)
		capacity = x->capacity * 2;
	answer = malloc(capacity);
	if (!answer)
		return (ERROR);
	ft_memcpy(answer, x->ans[x->cur], x->step);
	free(x->ans[x->cur]);
	x->ans[x->cur] = answer;
	x->capacity = capacity;
	return (SUCCESS);
}

int	append_move_to_soln(t_soln *x, char move)
{
	if (x == NULL)
		return (SUCCESS);
	if (x->step == x->capacity && grow_answer(x))
		return (ERROR);
	x->ans[x->cur][x->step++] = move;
	x->ans_len[x->cur] = x->step;
	return (SUCCESS);
}

int	soln_init(t_soln *x, const int soln_num, const int steps_limit)
{
	int	cur_soln;

	x->initial_capacity = steps_limit;
	x->capacity = steps_limit;
	x->cur = -1;
	x->step = -1;
	x->ans_len = ft_calloc(soln_num, sizeof(int));
	x->ans = ft_calloc(soln_num + 1, sizeof(char *));
	if (!x->ans || !x->ans_len)
		return (ERROR);
	cur_soln = 0;
	while (cur_soln < soln_num)
	{
		x->ans[cur_soln] = malloc(sizeof(char) * steps_limit);
		if (!x->ans[cur_soln])
			return (ERROR);
		cur_soln++;
	}
	return (SUCCESS);
}

int	new_soln_init(t_soln *x, t_circle_buf stacks[2],
	t_circle_buf *a_ori, t_circle_buf *b_ori)
{
	if (!x || !stacks || !a_ori || !b_ori)
		return (ERROR);
	if (a_ori->capacity > 501 || b_ori->capacity > 501)
		return (ERROR);
	x->capacity = x->initial_capacity;
	x->cur++;
	x->ans_len[x->cur] = 0;
	x->step = 0;
	ft_memcpy(&stacks[A], a_ori, sizeof(stacks[A]));
	ft_memcpy(&stacks[B], b_ori, sizeof(stacks[B]));
	return (SUCCESS);
}
