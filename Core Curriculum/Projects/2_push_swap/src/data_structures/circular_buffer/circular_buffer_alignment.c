/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   circular_buffer_alignment.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:42:07 by hnah              #+#    #+#             */
/*   Updated: 2026/10/01 18:47:06 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

/*
** O(1) alignment plan: A must be circularly ascending with all ranks 0..n-1.
** Empty A gives zero. Positive means ra, negative means rra; ties use rra.
** Return SUCCESS/ERROR; write the signed count without changing A.
*/
int	rot_a_min_rotation(t_circle_buf *a, int *rotations)
{
	int	top_rank;
	int	forward;

	if (!a || !rotations)
		return (ERROR);
	*rotations = 0;
	if (cbuf_len(a) == 0)
		return (SUCCESS);
	if (cbuf_read_at(a, 0, &top_rank) != SUCCESS)
		return (ERROR);
	forward = cbuf_opp_moves(a, top_rank);
	if (forward < top_rank)
		*rotations = forward;
	else
		*rotations = -top_rank;
	return (SUCCESS);
}

/*
** Bring A's minimum to its top by the cheaper rotation direction.
** The shared plan supplies the signed number of remaining moves.
** Requires circularly ascending A with all ranks 0..n-1 present.
** Empty, singleton or already aligned A needs no moves.
*/
int	rot_a_min_to_top(t_soln *x, t_circle_buf *a)
{
	int	rotations;
	int	outcome;

	if (rot_a_min_rotation(a, &rotations) == ERROR)
		return (ERROR);
	outcome = SUCCESS;
	while (rotations != 0 && outcome == SUCCESS)
	{
		if (rotations < 0)
			outcome = rra(x, a);
		else
			outcome = ra(x, a);
		if (outcome == SUCCESS)
			rotations -= ryker_ft_sign(rotations);
	}
	return (outcome);
}
