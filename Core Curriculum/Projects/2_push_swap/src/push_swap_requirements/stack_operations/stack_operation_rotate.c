/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stack_operation_rotate.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 11:10:33 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 17:56:32 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	ra(t_soln *x, t_circle_buf *a)
{
	append_move_to_soln(x, RA);
	return (cbuf_rotate(a));
}

int	rb(t_soln *x, t_circle_buf *b)
{
	append_move_to_soln(x, RB);
	return (cbuf_rotate(b));
}

int	rr(t_soln *x, t_circle_buf *a, t_circle_buf *b)
{
	append_move_to_soln(x, RR);
	return (cbuf_rotate(a) | cbuf_rotate(b));
}
