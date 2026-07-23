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

void	cbuf_info(cbuf *a, cbuf *b, int count)
{
	a->capacity = count + 1;
	a->read_idx = 0;
	a->write_idx = count;
	b->capacity = count + 1;
	b->read_idx = 0;
	b->write_idx = 0;	
	return ;
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

/*
int	cbuf_free(cbuf *a)
{
	if (a->buf)
	{
		free(a->buf);
		a->buf = NULL;
	}
	return (ERROR);
}
*/