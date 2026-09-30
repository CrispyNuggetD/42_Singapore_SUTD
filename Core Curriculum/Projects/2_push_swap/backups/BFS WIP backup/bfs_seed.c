/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs_seed.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:11:30 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 01:23:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bfs_seed.h"

static char	*seed_route(t_seed_search *search, int goal)
{
	char	*route;
	int		node;
	int		length;

	length = 0;
	node = goal;
	while (search->nodes[node].parent != -1)
	{
		length++;
		node = search->nodes[node].parent;
	}
	route = malloc(length + 1);
	if (!route)
		return (NULL);
	route[length] = 0;
	while (search->nodes[goal].parent != -1)
	{
		route[--length] = search->nodes[goal].move;
		goal = search->nodes[goal].parent;
	}
	return (route);
}

static int	seed_move(soln *x, circle_buf *a, circle_buf *b, char move)
{
	if (move == SA)
		return (sa(x, a));
	if (move == SB)
		return (sb(x, b));
	if (move == PA)
		return (pa(x, a, b));
	if (move == PB)
		return (pb(x, a, b));
	if (move == RA)
		return (ra(x, a));
	if (move == RRA)
		return (rra(x, a));
	return (ERROR);
}

int	seed_replay(soln *x, circle_buf *a, circle_buf *b, char *route)
{
	int	i;

	i = 0;
	while (route[i])
	{
		if (seed_move(x, a, b, route[i++]) == ERROR)
			return (ERROR);
	}
	return (SUCCESS);
}

/* Append a seed route to the current solution; preserve the existing B tail. */
int	bfs_seed_sort(soln *x, circle_buf *a, circle_buf *b)
{
	t_seed_search	search;
	char			*route;
	int				goal;
	int				outcome;

	if (cbuf_len(a) > BRUTE_MAX_N)
		return (ERROR);
	if (cbuf_len(a) < 2)
		return (SUCCESS);
	if (seed_search_init(&search, a) == ERROR)
		return (ERROR);
	goal = seed_search_goal(&search);
	route = NULL;
	if (goal >= 0)
		route = seed_route(&search, goal);
	free(search.nodes);
	free(search.visited);
	if (!route)
		return (ERROR);
	outcome = seed_replay(x, a, b, route);
	free(route);
	return (outcome);
}
