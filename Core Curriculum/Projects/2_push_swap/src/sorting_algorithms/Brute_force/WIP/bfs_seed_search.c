/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs_seed_search.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:11:30 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 01:23:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bfs_seed.h"

/*
** B has an unmodelled tail. Never rotate B or access below its seed prefix.
** Model no-ops are discarded before replay, including pa on empty model B.
*/
static int	seed_discover(t_seed_search *search, char move)
{
	t_brutestate	state;
	int				id;
	int				next;

	state = search->nodes[search->head].state;
	brute_apply_move(&state, move, search->n);
	id = calculate_state_id(&state, search->n);
	if (search->visited[id / 8] & (1 << (id % 8)))
		return (-1);
	search->visited[id / 8] |= 1 << (id % 8);
	next = search->count++;
	search->nodes[next].state = state;
	search->nodes[next].parent = search->head;
	search->nodes[next].move = move;
	return (next);
}

/* Shortest route within sa/sb/pa/pb/ra/rra; this is not full-stack BFS. */
int	seed_search_goal(t_seed_search *search)
{
	const char	*moves;
	int			i;
	int			next;

	moves = "124569";
	if (seed_is_goal(&search->nodes[0].state, search->n))
		return (0);
	while (search->head < search->count)
	{
		i = 0;
		while (moves[i])
		{
			next = seed_discover(search, moves[i++]);
			if (next >= 0
				&& seed_is_goal(&search->nodes[next].state, search->n))
				return (next);
		}
		search->head++;
	}
	return (-1);
}
