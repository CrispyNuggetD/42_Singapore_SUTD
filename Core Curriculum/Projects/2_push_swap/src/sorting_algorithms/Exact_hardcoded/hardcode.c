/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hardcode.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 16:29:49 by hnah              #+#    #+#             */
/*   Updated: 2026/09/28 16:52:47 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	hardcode_three(soln *x, circle_buf *a)
{
	int	a_size;
	int	pattern;

	a_size = cbuf_len(a);
	if (a_size == 3)
	{
		pattern = get_order_top_three(a);
		if (pattern == 321)
		{
			if (sa(x, a) == ERROR)
				return (ERROR);
			return (rra(x, a));
		}
		else if (pattern == 312)
			return (ra(x, a));
		else if (pattern == 231)
			return (rra(x, a));
		else if (pattern == 213)
			return (sa(x, a));
		else if (pattern == 132)
		{
			if (sa(x, a) == ERROR)
				return (ERROR);
			return (ra(x, a));
		}
	}
	return (SUCCESS);
}
