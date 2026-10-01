/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs_state_representation.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/10 20:28:42 by hnah              #+#    #+#             */
/*   Updated: 2026/10/01 18:04:03 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

/*
** Build a compact BFS state from circular buffers without changing the stacks.
** value[] stores A then B, each top to bottom; split marks the start of B.
*/

/* 
** Wait, "can't you just use your cbuf?" Short answer: memory + indexing.
** BFS keeps many candidate states. 
** Two cbufs would copy TWO 501-int arrays and their indices into every node.
** This keeps only the small ranked sequence and split, saving node memory.
** The flat order ALSO makes Lehmer indexing independent of buffer wraparound.
** The tradeoff is separate BFS operations on this compact representation.
*/

/* Copy top-to-bottom order, including circular-buffer wraparound. */
static void	copy_stack_values(unsigned char *dest, t_circle_buf *stack)
{
	int	offset;
	int	length;

	length = cbuf_len(stack);
	offset = 0;
	while (offset < length)
	{
		dest[offset] = stack->buf[(stack->read_idx + offset) % stack->capacity];
		offset++;
	}
}

/* Requires normalised ranks and len(A) + len(B) <= BRUTE_MAX_N. */
void	gen_brute_state(t_brutestate *state, t_circle_buf *a, t_circle_buf *b)
{
	state->split = cbuf_len(a);
	copy_stack_values(state->value, a);
	copy_stack_values(state->value + state->split, b);
}
