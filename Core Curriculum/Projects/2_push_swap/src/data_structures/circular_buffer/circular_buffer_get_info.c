/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   circular_buffer_get_info.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 14:49:00 by hnah              #+#    #+#             */
/*   Updated: 2026/10/01 19:03:47 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"
#include "circular_buffer_lis.h"

/* Return the top three's relative order; requires three distinct values. */
int	get_order_top_three(t_circle_buf *a)
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
** Return the nonnegative move count in the opposite rotation direction.
** Accept a valid, initialised stack and signed moves with |moves| < length.
** Return length - |moves|; zero moves or an empty stack returns zero.
** The result is a magnitude, not a signed rotation plan. No stack mutation.
*/
int	cbuf_opp_moves(t_circle_buf *stack, int moves)
{
	int	stack_len;

	stack_len = cbuf_len(stack);
	if (stack_len == 0 || moves == 0)
		return (0);
	else if (moves < 0)
		return (stack_len + moves);
	else
		return (stack_len - moves);
}

/* Calculate LIS lengths and predecessors for one circular starting position. */
static void	calculate_lis_trial(t_circle_buf *stack, t_cbuf_lis *lis)
{
	int	cur_elem;
	int	prev_elem;
	int	values[2];

	cur_elem = -1;
	while (++cur_elem < lis->count)
	{
		lis->length[cur_elem] = 1;
		lis->previous[cur_elem] = -1;
		cbuf_read_at(stack, (lis->start + cur_elem) % lis->count, &values[1]);
		prev_elem = -1;
		while (++prev_elem < cur_elem)
		{
			cbuf_read_at(stack, (lis->start + prev_elem) % lis->count,
				&values[0]);
			if (values[0] < values[1]
				&& lis->length[prev_elem] + 1 > lis->length[cur_elem])
			{
				lis->length[cur_elem] = lis->length[prev_elem] + 1;
				lis->previous[cur_elem] = prev_elem;
			}
		}
	}
}

/* Replace the flags only for a longer sequence; ties keep the earlier trial. */
static void	save_best_lis(t_cbuf_lis *lis, char keep_flags[500])
{
	int	trial_max_index;
	int	trial_length;

	trial_max_index = ryker_ft_array_max_at(lis->length, &trial_length,
			lis->count);
	if (trial_length <= lis->best_length)
		return ;
	lis->best_length = trial_length;
	ft_memset(keep_flags, 0, lis->count);
	while (trial_max_index != -1)
	{
		keep_flags[(lis->start + trial_max_index) % lis->count] = 1;
		trial_max_index = lis->previous[trial_max_index];
	}
}

/*
** Mark a longest circular increasing subsequence by logical stack position.
** Stack is unchanged; empty input leaves flags untouched. At most 500 values.
*/
int	get_cbuf_lis(t_circle_buf *stack, char keep_flags[500])
{
	t_cbuf_lis	lis;

	lis.count = cbuf_len(stack);
	if (lis.count == 0)
		return (SUCCESS);
	lis.start = 0;
	lis.best_length = 0;
	while (lis.start < lis.count)
	{
		calculate_lis_trial(stack, &lis);
		save_best_lis(&lis, keep_flags);
		lis.start++;
	}
	debug_lis_length(lis.best_length);
	return (SUCCESS);
}
