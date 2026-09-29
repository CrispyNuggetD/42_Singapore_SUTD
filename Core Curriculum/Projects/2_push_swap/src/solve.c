/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   solve.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:11:30 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 01:23:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

/* Each candidate owns a fresh copy; preparation never resets a solution. */
int	solve(soln *x, circle_buf *a, circle_buf *b, int count)
{
	circle_buf	stacks[2];
	t_algorithm	algo;

	if (count != cbuf_len(a) || cbuf_len(b) != 0)
		return (ERROR);
	algo = ALGO_THREE_LOCAL;
	while (algo < ALGO_COUNT)
	{
		if (new_soln_init(x, stacks, a, b) == ERROR)
			return (ERROR);
		if (greedy_reinsertion(x, &stacks[A], &stacks[B], algo) == ERROR)
			return (ERROR);
		algo++;
	}
	return (SUCCESS);
}
