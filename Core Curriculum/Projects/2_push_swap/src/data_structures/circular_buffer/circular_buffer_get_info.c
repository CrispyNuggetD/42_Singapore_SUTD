/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   circular_buffer_get_info.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 14:49:00 by hnah              #+#    #+#             */
/*   Updated: 2026/09/28 22:36:59 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

/*
** Return the nonnegative move count in the opposite rotation direction.
** Accept a valid, initialised stack and signed moves with |moves| < length.
** Return length - |moves|; zero moves or an empty stack returns zero.
** The result is a magnitude, not a signed rotation plan. No stack mutation.
*/
int	cbuf_opp_moves(circle_buf *stack, int moves)
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

int cbuf_lis(circle_buf *stack, char keep_flags[500])
{
	int	stack_len;
	int	cur_elem;
	int	prev_elem;
	int	length[500];
	int	previous[500];
	int values[3];
	int trial_max_index;
	int start;
	int best_circular_lis_len;

	stack_len = cbuf_len(stack);
	if (stack_len == 0)
		return (SUCCESS);
	start = 0;
	best_circular_lis_len = 0;
	while(start < stack_len)
	{
		cur_elem = 0;
		while (cur_elem < stack_len)
		{
			length[cur_elem] = 1;
			previous[cur_elem] = -1;
			cbuf_read_at(stack, (start + cur_elem) % stack_len, &values[1]);
			prev_elem = 0;
			while (prev_elem < cur_elem)
			{
				cbuf_read_at(stack, (start + prev_elem) % stack_len, &values[0]);
				if (values[0] < values[1])
				{
					if (length[prev_elem] + 1 > length[cur_elem])
					{
						length[cur_elem] = length[prev_elem] + 1;
						previous[cur_elem] = prev_elem;
					}
				}
				prev_elem++;
			}
			cur_elem++;
		}
		trial_max_index = ryker_ft_array_max_at(length, &values[2], stack_len);
		if (values[2] > best_circular_lis_len)
		{
			best_circular_lis_len = values[2];
			ft_memset(keep_flags, 0, stack_len);
			while (trial_max_index != -1)
			{
				keep_flags[(start + trial_max_index) % stack_len] = 1;
				trial_max_index = previous[trial_max_index];
			}
		}
		start++;
	}
	ryker_ft_printf_fd(2, "best_circular_lis_len: %i\n", best_circular_lis_len);
	return (SUCCESS);
}
