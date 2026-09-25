/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   circular_buffer_get_info.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 14:49:00 by hnah              #+#    #+#             */
/*   Updated: 2026/09/25 16:01:42 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	cbuf_rev_moves(circle_buf *stack, int moves)
{
	int	stack_len = cbuf_len(stack);
	if (stack_len == 0 || moves == 0)
		return (0);
	else if (moves < 0)
		return (stack_len + moves);
	else
		return (stack_len - moves);
}
