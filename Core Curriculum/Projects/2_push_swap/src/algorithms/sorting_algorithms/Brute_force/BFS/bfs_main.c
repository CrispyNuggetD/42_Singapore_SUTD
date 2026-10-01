/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs_main.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/10 20:28:42 by hnah              #+#    #+#             */
/*   Updated: 2026/10/01 18:53:34 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bfs.h"

/*
** Walk parent links backwards and write the route in forward execution order.
** Replace the current solution route and length; do not apply moves to stacks.
*/
static void	reconstruct_brute_path(t_soln *x, t_brutenode *nodes, int goal)
{
	int	node;
	int	len;
	int	pos;

	node = goal;
	len = 0;
	while (nodes[node].parent != -1)
	{
		len++;
		node = nodes[node].parent;
	}
	node = goal;
	pos = len - 1;
	while (nodes[node].parent != -1)
	{
		x->ans[x->cur][pos] = nodes[node].move;
		pos--;
		node = nodes[node].parent;
	}
	x->ans_len[x->cur] = len;
	x->step = len;
}

/* Allocate the node queue and report its planned memory usage. */
static t_brutenode	*allocate_nodes(int count)
{
	t_brutenode	*nodes;
	int			capacity;

	capacity = bfs_possible_states(count);
	debug_bfs_start(count, capacity);
	debug_print_bfs_memory(capacity);
	debug_bfs_alloc(sizeof(t_brutenode) * (size_t)capacity);
	nodes = malloc(sizeof(t_brutenode) * capacity);
	if (nodes)
		debug_print_message("BFS ALLOCATION READY");
	return (nodes);
}

/*
** Allocate nodes for (count + 1)! states and record a shortest sorting route.
** Replace the active answer; leave A and B unchanged.
** Candidate comparison uses the recorded answer without replaying it.
** A chunk caller must replay the route if it needs to update its real stacks.
** Requires count == len(A), empty B, and ranks 0..count-1 within BRUTE_MAX_N.
** Callers enforce these entry conditions; a chunk must use local ranks.
** Goal: ascending A and empty B. Diagnostics follow DEBUG and use stderr.
*/
int	brute_solve(t_soln *x, t_circle_buf *a, t_circle_buf *b, int count)
{
	t_brutenode	*nodes;
	int			goal;

	nodes = allocate_nodes(count);
	if (!nodes)
	{
		debug_bfs_end(-1);
		return (ERROR);
	}
	goal = bfs_find_goal(nodes, a, b);
	if (goal >= 0)
		reconstruct_brute_path(x, nodes, goal);
	free(nodes);
	if (goal < 0)
	{
		debug_bfs_end(-1);
		return (ERROR);
	}
	debug_bfs_end(x->step);
	return (SUCCESS);
}
