/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   greedy_stages.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:11:30 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 02:27:38 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"

/* Preparation leaves A circularly ascending; insertion preserves that. */
int	greedy_insert_all(soln *x, circle_buf *a, circle_buf *b)
{
	t_greedy_plan	best;

	while (cbuf_len(b) > 0)
	{
		if (greedy_choose_plan(a, b, &best) == ERROR)
			return (ERROR);
		if (greedy_execute_plan(x, a, b, &best) == ERROR)
			return (ERROR);
	}
	return (SUCCESS);
}

int	greedy_reinsertion(soln *x, circle_buf *a, circle_buf *b,
	t_seed_mode mode)
{
	if (greedy_prepare(x, a, b, mode) == ERROR)
		return (ERROR);
	if (greedy_insert_all(x, a, b) == ERROR)
		return (ERROR);
	return (rot_a_min_to_top(x, a));
}
