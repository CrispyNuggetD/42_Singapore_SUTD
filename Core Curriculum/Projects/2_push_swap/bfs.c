/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/10 20:28:42 by hnah              #+#    #+#             */
/*   Updated: 2026/08/24 17:10:40 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

#ifdef BFS_DEBUG
# include "debug/DO_NOT_SUBMIT_DEBUG_bfs_results.h"
#endif

static void	gen_brute_state(t_brutestate *state, cbuf *a, cbuf *b)
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

static int	move_hits_hidden_a(t_brutestate *state, char move)
{
	if (state->split < 2 && move == SA)
		return (1);
	if (state->split > 0 && (move == RA || move == RRA))
		return (1);
	return (0);
}

static int	brute_apply_wall_move(t_brutestate *state, char move, int n)
{
	if (move_hits_hidden_a(state, move))
		return (0);
	brute_apply_move(state, move, n);
	return (1);
}

static int	state_was_visited(unsigned char *visited, int state_id)
{
	return (visited[state_id / 8] & (1 << (state_id % 8)));
}

static void	mark_state_visited(unsigned char *visited, int state_id)
{
	visited[state_id / 8] |= (1 << (state_id % 8));
}

//

/* later: check duplicate upgrade to Lehmer */
/* A no-op such as sb when B is empty is handled automatically: 
temp remains identical to nodes[i].state, so brute_state_exists() finds it and it isn't appended. */
static int	bfs_find_goal(t_brutenode *nodes, cbuf *a, cbuf *b)
{
	t_brutestate	temp;
	int			n;
	int			i;
	int			total;
	int			move_to_try;
	int			state_id;
	unsigned char	*visited;
	char	moves[6] = {SA, SB, PA, PB, RA, RRA};

	n = cbuf_len(a) + cbuf_len(b);
	if (n > BRUTE_MAX_N)
		return (ERROR);
	visited = ft_calloc((BRUTE_TOTAL_N_PLUS_1_FACTORIAL + 7) / 8,
			sizeof(unsigned char));
	if (!visited)
		return (ERROR);
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
		move_to_try = 0;
		while (move_to_try < 6)
		{
			temp = nodes[i].state;
			if (!brute_apply_wall_move(&temp, moves[move_to_try], n))
			{
				move_to_try++;
				continue ;
			}
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

int	brute_solve(soln *x, cbuf *a, cbuf *b)
{
	t_brutenode	*nodes;
	int			goal;

	nodes = malloc(sizeof(t_brutenode) * BRUTE_TOTAL_N_PLUS_1_FACTORIAL);
	if (!nodes)
		return (ERROR);
	
	goal = bfs_find_goal(nodes, a, b);
	if (goal < 0)
	{
		free(nodes);
		return (ERROR);
	}
	reconstruct_brute_path(x, nodes, goal);
	free(nodes);
	return (SUCCESS);
}

/* int	brute_solve(soln *x, cbuf *a, cbuf *b)
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
