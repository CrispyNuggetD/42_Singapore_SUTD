/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stack_operation_swap_push.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 11:10:33 by hnah              #+#    #+#             */
/*   Updated: 2026/09/23 22:09:59 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	sa(t_soln *x, t_circle_buf *a)
{
	append_move_to_soln(x, SA);
	return (cbuf_swap_top(a));
}

int	sb(t_soln *x, t_circle_buf *b)
{
	append_move_to_soln(x, SB);
	return (cbuf_swap_top(b));
}

int	ss(t_soln *x, t_circle_buf *a, t_circle_buf *b)
{
	append_move_to_soln(x, SS);
	return (cbuf_swap_top(a) | cbuf_swap_top(b));
}

int	pa(t_soln *x, t_circle_buf *a, t_circle_buf *b)
{
	int	read_number;

	append_move_to_soln(x, PA);
	if (cbuf_pop_top(b, &read_number) == ERROR)
		return (ERROR);
	return (cbuf_push_top(a, read_number));
}

int	pb(t_soln *x, t_circle_buf *a, t_circle_buf *b)
{
	int	read_number;

	append_move_to_soln(x, PB);
	if (cbuf_pop_top(a, &read_number) == ERROR)
		return (ERROR);
	return (cbuf_push_top(b, read_number));
}
