/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs_seed_wip.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:11:30 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 01:23:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bfs_seed_wip.h"

/* Goal concerns only A. Seed membership and B order may change. */
int	wip_bfs_seed_is_goal(circle_buf *a)
{
	int	i;
	int	previous;
	int	current;

	if (!a || cbuf_len(a) != WIP_SEED_SIZE)
		return (0);
	if (cbuf_read_at(a, 0, &previous) == ERROR)
		return (0);
	i = 1;
	while (i < WIP_SEED_SIZE)
	{
		if (cbuf_read_at(a, i++, &current) == ERROR)
			return (0);
		if (previous >= current)
			return (0);
		previous = current;
	}
	return (1);
}

/*
** WIP: no allocation, search, replay, or changes to the supplied solution.
** TODO: bounded FIFO, all 11 moves, exact deduplication, goal, route replay.
** See README.md for the intended contract and implementation checkpoints.
*/
t_wip_bfs_status	wip_bfs_seed_search(soln *x, circle_buf *a,
	circle_buf *b, const t_wip_bfs_limits *limits)
{
	(void)x;
	(void)a;
	(void)b;
	(void)limits;
	return (WIP_BFS_NOT_IMPLEMENTED);
}
