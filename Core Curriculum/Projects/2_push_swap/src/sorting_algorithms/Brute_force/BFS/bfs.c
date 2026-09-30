/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/10 20:28:42 by hnah              #+#    #+#             */
/*   Updated: 2026/09/30 19:51:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

#ifdef BFS_DEBUG
# include "DO_NOT_SUBMIT_DEBUG_bfs_results.h"
#endif

/*
** Flatten A then B, top to bottom, into value[]; split records the size of A.
** Copy ranks without renormalising them; callers must supply small local ranks.
*/
static void	gen_brute_state(t_brutestate *state, circle_buf *a, circle_buf *b)
{
	int	offset;
	int	index;
	int	pos;
	int	a_len;
	int	b_len;

	a_len = cbuf_len(a);
	b_len = cbuf_len(b);
	state->split = a_len;
	pos = 0;
	offset = 0;
	while (offset < a_len)
	{
		index = (a->read_idx + offset) % a->capacity;
		state->value[pos] = a->buf[index];
		pos++;
		offset++;
	}
	offset = 0;
	while (offset < b_len)
	{
		index = (b->read_idx + offset) % b->capacity;
		state->value[pos] = b->buf[index];
		pos++;
		offset++;
	}
}

//

/*
** Read the visited bit for this encoded state; nonzero means already discovered.
*/
static int	state_was_visited(unsigned char *visited, int state_id)
{
	return (visited[state_id / 8] & (1 << (state_id % 8)));
}

/*
** Set the visited bit so later routes to the same state are skipped.
*/
static void	mark_state_visited(unsigned char *visited, int state_id)
{
	visited[state_id / 8] |= (1 << (state_id % 8));
}

//

/*
** Explore all eleven moves in FIFO order, with every input element modelled.
** Lehmer state IDs index visited bits; repeated states and no-ops are skipped.
** Each discovered node stores its parent and the encoded move used to reach it.
** Return the first goal's node index (a shortest route), or -1 on failure.
** The goal is ascending A and empty B; a sorted initial state returns index 0.
*/
static int	bfs_find_goal(t_brutenode *nodes, circle_buf *a, circle_buf *b)
{
	t_brutestate	temp;
	int				n;
	int				i;
	int				total;
	int				move_to_try;
	int				state_id;
	unsigned char	*visited;
	char			moves[12] = "123456789AB";

	n = cbuf_len(a);
	if (n > BRUTE_MAX_N)
		return (-1);
	visited = ft_calloc((bfs_possible_states(n) + 7) / 8,
			sizeof(unsigned char));
	if (!visited)
		return (-1);
	gen_brute_state(&nodes[0].state, a, b);
	mark_state_visited(visited, calculate_state_id(&nodes[0].state, n));
	nodes[0].parent = -1;
	nodes[0].move = 0;
	if (is_brute_goal(&nodes[0].state, n))
	{
		free(visited);
		return (0);
	}
	i = 0;
	total = 1;
	while (i < total)
	{
		if ((i & 32767) == 0)
			debug_bfs_progress(i, total, bfs_possible_states(n));
		move_to_try = 0;
		while (move_to_try < 11)
		{
			temp = nodes[i].state;
			brute_apply_move(&temp, moves[move_to_try], n);
			state_id = calculate_state_id(&temp, n);
			if (!state_was_visited(visited, state_id))
			{
				mark_state_visited(visited, state_id);
				nodes[total].state = temp;
				nodes[total].parent = i;
				nodes[total].move = moves[move_to_try];
				if (is_brute_goal(&nodes[total].state, n))
				{
					free(visited);
					return (total);
				}
				total++;
			}
			move_to_try++;
		}
		i++;
	}
	free(visited);
	return (-1);
}

/*
** Walk parent links backwards and write the route in forward execution order.
** Replace the current solution route and length; do not apply moves to stacks.
*/
static void	reconstruct_brute_path(soln *x, t_brutenode *nodes, int goal)
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

/*
** Allocate the configured maximum node table, search, and reconstruct a route.
** Leave the input stacks unchanged; callers must replay the route themselves.
** Goal: ascending A and empty B. Diagnostics follow DEBUG and use stderr.
** Intended entry: B empty, at most BRUTE_MAX_N normalised ranks in A.
** The empty-B entry check is not yet enforced here.
*/
int	brute_solve(soln *x, circle_buf *a, circle_buf *b, int count)
{
	t_brutenode	*nodes;
	int			goal;

	debug_bfs_start(count, bfs_possible_states(count));
	debug_print_bfs_memory(bfs_possible_states(count));
	debug_bfs_alloc(
		sizeof(t_brutenode) * (size_t)bfs_possible_states(count));

	nodes = malloc(sizeof(t_brutenode) * bfs_possible_states(count));
	if (!nodes)
	{
		debug_bfs_end(-1);

		return (ERROR);
	}
	debug_print_message("BFS ALLOCATION READY");

	
	goal = bfs_find_goal(nodes, a, b);
	if (goal < 0)
	{
		debug_bfs_end(-1);
		free(nodes);
		return (ERROR);
	}
	reconstruct_brute_path(x, nodes, goal);
	debug_bfs_end(x->step);
	free(nodes);
	return (SUCCESS);
}

/* int	brute_solve(soln *x, circle_buf *a, circle_buf *b)
{
	t_brutenode	nodes[BRUTE_TOTAL_N_PLUS_1_FACTORIAL];
	int			goal;

	goal = bfs_find_goal(nodes, a, b);
	if (goal < 0)
		return (ERROR);
	reconstruct_brute_path(x, nodes, goal);
#ifdef BFS_DEBUG
	debug_log_bfs_run(nodes[0].state.value,
		cbuf_len(a) + cbuf_len(b), x->ans[x->cur], x->ans_len[x->cur]);
#endif
	return (SUCCESS);
} */
