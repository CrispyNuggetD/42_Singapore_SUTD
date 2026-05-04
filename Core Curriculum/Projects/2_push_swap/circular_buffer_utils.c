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

int cbuf_init(cbuf *stack, int size)
{
	stack->buffer = malloc(sizeof(int) * (size + 1));
	if (!stack->buffer)
		return (ERROR);
	stack->capacity = size;
	stack->read_idx = 0;
	stack->write_idx = 0;
	return (SUCCESS);
}

int	cbuf_len(cbuf *stack)
{
	return ((stack->write_idx - stack->read_idx + stack->capacity) \
% stack->capacity);
}

int	cbuf_is_empty(cbuf *stack)
{
	return (stack->read_idx == stack->write_idx);
}

int	cbuf_is_full(cbuf *stack)
{
	return ((stack->write_idx + 1) % stack->capacity == stack->read_idx);
}

void	cbuf_free(cbuf *stack)
{
	free(stack->buffer);
	stack->buffer = NULL;
}