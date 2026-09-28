/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stack_operation_rotate.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 11:10:33 by hnah              #+#    #+#             */
/*   Updated: 2026/09/23 22:09:59 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	ra(soln *x, circle_buf *a)
{
	append_move_to_soln(x, RA);
	return (cbuf_rotate(a));
}

int	rb(soln *x, circle_buf *b)
{
	append_move_to_soln(x, RB);
	return (cbuf_rotate(b));
}

int	rr(soln *x, circle_buf *a, circle_buf *b)
{
	append_move_to_soln(x, RR);
	return (cbuf_rotate(a) | cbuf_rotate(b));
}
