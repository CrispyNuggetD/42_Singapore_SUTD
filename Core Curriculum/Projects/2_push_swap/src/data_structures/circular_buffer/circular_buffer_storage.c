/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   circular_buffer_storage.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:42:07 by hnah              #+#    #+#             */
/*   Updated: 2026/10/01 18:47:46 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

/* Inline buffers keep recursive struct copies independent without malloc. */
int	*cbuf_data(t_circle_buf *stack)
{
	if (stack->capacity <= 501)
		return (stack->buf);
	return (stack->large_buf);
}

/* Call once on fresh stacks. Caller frees both pointers, even on failure. */
int	cbuf_allocate_ab(t_circle_buf *a, t_circle_buf *b, int count)
{
	cbuf_init_ab(a, b, count);
	if (count <= 500)
		return (SUCCESS);
	if ((size_t)a->capacity > (size_t)-1 / sizeof(int))
		return (ERROR);
	a->large_buf = malloc(sizeof(int) * (size_t)a->capacity);
	b->large_buf = malloc(sizeof(int) * (size_t)b->capacity);
	if (!a->large_buf || !b->large_buf)
		return (ERROR);
	return (SUCCESS);
}
