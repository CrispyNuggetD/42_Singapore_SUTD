/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs_search.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/10 20:28:42 by hnah              #+#    #+#             */
/*   Updated: 2026/10/01 18:16:43 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bfs.h"

/* Seed the queue and visited set. The caller owns nodes and frees visited. */
static int	init_search(t_bfs_search *search, t_circle_buf *a, t_circle_buf *b)
{
	search->count = cbuf_len(a);
	if (search->count > BRUTE_MAX_N)
		return (ERROR);
	search->capacity = bfs_possible_states(search->count);
	search->visited = ft_calloc((search->capacity + 7) / 8,
			sizeof(unsigned char));
	if (!search->visited)
		return (ERROR);
	gen_brute_state(&search->nodes[0].state, a, b);
	search->nodes[0].parent = -1;
	search->nodes[0].move = 0;
	mark_state_visited(search->visited,
		calculate_state_id(&search->nodes[0].state, search->count));
	search->head = 0;
	search->total = 1;
	return (SUCCESS);
}

/* Append an unseen neighbour. Return its index if sorted, otherwise -1. */
static int	try_move(t_bfs_search *search, char move)
{
	t_brutestate	state;
	int				state_id;
	int				child;

	state = search->nodes[search->head].state;
	brute_apply_move(&state, move, search->count);
	state_id = calculate_state_id(&state, search->count);
	if (state_was_visited(search->visited, state_id))
		return (-1);
	mark_state_visited(search->visited, state_id);
	child = search->total;
	search->nodes[child].state = state;
	search->nodes[child].parent = search->head;
	search->nodes[child].move = move;
	search->total++;
	if (is_brute_goal(&state, search->count))
		return (child);
	return (-1);
}

/* Process nodes in discovery order; the first goal has a shortest route. */
static int	search_queue(t_bfs_search *search)
{
	static const char	moves[] = "123456789AB";
	int					move_index;
	int					goal;

	while (search->head < search->total)
	{
		if (search->head % BFS_PROGRESS_INTERVAL == 0)
			debug_bfs_progress(search->head, search->total, search->capacity);
		move_index = 0;
		while (moves[move_index])
		{
			goal = try_move(search, moves[move_index]);
			if (goal >= 0)
				return (goal);
			move_index++;
		}
		search->head++;
	}
	return (-1);
}

/* Return the goal node, or -1 on failure. A and B are never modified. */
int	bfs_find_goal(t_brutenode *nodes, t_circle_buf *a, t_circle_buf *b)
{
	t_bfs_search	search;
	int				goal;

	search.nodes = nodes;
	if (init_search(&search, a, b) == ERROR)
		return (-1);
	if (is_brute_goal(&nodes[0].state, search.count))
		goal = 0;
	else
		goal = search_queue(&search);
	free(search.visited);
	return (goal);
}
