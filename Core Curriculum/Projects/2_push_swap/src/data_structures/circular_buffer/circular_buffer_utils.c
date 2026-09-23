/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   circular_buffer_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:42:07 by hnah              #+#    #+#             */
/*   Updated: 2026/09/23 21:50:08 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	cbuf_init_ab(circle_buf *a, circle_buf *b, int count)
{
	a->capacity = count + 1;
	a->read_idx = 0;
	a->write_idx = count;
	b->capacity = count + 1;
	b->read_idx = 0;
	b->write_idx = 0;	
	return ;
}

/* Read a logical position from the top without changing the stack. */
int	cbuf_read_at(circle_buf *stack, int read_index, int *value)
{
	int len;
	
	if (!stack || !value)
		return (ERROR);
	len = cbuf_len(stack);
	if (read_index <= -len || read_index >= len)
		return (ERROR);
	if (read_index < 0)
		read_index += len;
	*value = stack->buf[(stack->read_idx + read_index) % stack->capacity];
	return (SUCCESS);
}

int	cbuf_len(circle_buf *stack)
{
	return ((stack->write_idx - stack->read_idx + stack->capacity) \
% stack->capacity);
}

int	cbuf_is_empty(circle_buf *stack)
{
	return (stack->read_idx == stack->write_idx);
}

int	cbuf_is_full(circle_buf *stack)
{
	return ((stack->write_idx + 1) % stack->capacity == stack->read_idx);
}
