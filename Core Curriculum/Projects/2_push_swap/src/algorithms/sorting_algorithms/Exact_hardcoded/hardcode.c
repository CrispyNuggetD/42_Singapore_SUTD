/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hardcode.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 16:29:49 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 02:26:03 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	hardcode_three(t_soln *x, t_circle_buf *a)
{
	int	pattern;

	if (cbuf_len(a) != 3)
		return (SUCCESS);
	pattern = get_order_top_three(a);
	if (pattern == 321 || pattern == 132 || pattern == 213)
	{
		if (sa(x, a) == ERROR)
			return (ERROR);
	}
	if (pattern == 321 || pattern == 231)
		return (rra(x, a));
	if (pattern == 312 || pattern == 132)
		return (ra(x, a));
	return (SUCCESS);
}
