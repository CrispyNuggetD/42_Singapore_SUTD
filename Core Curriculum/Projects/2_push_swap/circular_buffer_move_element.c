/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   circular_buffer_move_element.c                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:42:13 by hnah              #+#    #+#             */
/*   Updated: 2026/04/23 13:46:19 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	cbuf_rotate(cbuf *stack)
{
	int	top_idx;
	int	insert_idx;
	int	cbuf_cap;
	
	if (cbuf_len(stack) < 2)
		return (SUCCESS);
	top_idx = stack->read_idx;
	insert_idx = stack->write_idx;
	cbuf_cap = stack->capacity;
	stack->buf[insert_idx] = stack->buf[top_idx];
	stack->read_idx = (stack->read_idx + 1) % cbuf_cap;
	stack->write_idx = (stack->write_idx + 1) % cbuf_cap;
	return (SUCCESS);
}

int	cbuf_rev_rotate(cbuf *stack)
{
	int	top_idx;
	int	cbuf_cap;

	if (cbuf_len(stack) < 2)
		return (SUCCESS);
	top_idx = stack->read_idx;
	cbuf_cap = stack->capacity;
	stack->write_idx = (stack->write_idx - 1 + cbuf_cap) % cbuf_cap;
	stack->buf[top_idx] = stack->buf[write_idx];
	stack->read_idx = (stack->read_idx - 1 + cbuf_cap) % cbuf_cap;
	return (SUCCESS);
}

int	cbuf_swap_top(cbuf *stack)
{
	int	first_idx;
	int	second_idx;
	int	temp_storage;

	if (cbuf_len(stack) < 2)
		return (SKIP);
	first_idx = stack->read_idx;
	second_idx = (stack->read_idx + 1) % stack->capacity;
	temp_storage = stack->buf[first_idx];
	stack->buf[first_idx] = stack->buf[second_idx];
	stack->buf[second_idx] = temp_storage;
	return (SUCCESS);
}
