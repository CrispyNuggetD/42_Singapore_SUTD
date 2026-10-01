/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   circular_buffer_insert_remove.c                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:42:11 by hnah              #+#    #+#             */
/*   Updated: 2026/05/04 19:04:39 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	cbuf_push_top(t_circle_buf *stack, int number)
{
	int	cbuf_cap;

	cbuf_cap = stack->capacity;
	if (cbuf_is_full(stack))
		return (ERROR);
	stack->read_idx = ((size_t)stack->read_idx + cbuf_cap - 1) % cbuf_cap;
	cbuf_data(stack)[stack->read_idx] = number;
	return (SUCCESS);
}

int	cbuf_push_bottom(t_circle_buf *stack, int number)
{
	int	cbuf_cap;

	cbuf_cap = stack->capacity;
	if (cbuf_is_full(stack))
		return (ERROR);
	cbuf_data(stack)[stack->write_idx] = number;
	stack->write_idx = (stack->write_idx + 1) % cbuf_cap;
	return (SUCCESS);
}

int	cbuf_pop_top(t_circle_buf *stack, int *pop_number)
{
	int	cbuf_cap;

	cbuf_cap = stack->capacity;
	if (cbuf_is_empty(stack))
		return (ERROR);
	*pop_number = cbuf_data(stack)[stack->read_idx];
	stack->read_idx = (stack->read_idx + 1) % cbuf_cap;
	return (SUCCESS);
}

int	cbuf_pop_bottom(t_circle_buf *stack, int *pop_number)
{
	int	cbuf_cap;

	cbuf_cap = stack->capacity;
	if (cbuf_is_empty(stack))
		return (ERROR);
	stack->write_idx = ((size_t)stack->write_idx + cbuf_cap - 1) % cbuf_cap;
	*pop_number = cbuf_data(stack)[stack->write_idx];
	return (SUCCESS);
}
