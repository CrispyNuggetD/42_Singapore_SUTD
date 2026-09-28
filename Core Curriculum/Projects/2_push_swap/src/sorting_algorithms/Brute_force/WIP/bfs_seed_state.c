/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs_seed_state.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:11:30 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 01:23:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bfs_seed.h"

/* Normalise only the seed: global input ranks may be greater than 255. */
static void	seed_rank(t_brutestate *state, circle_buf *a, int n)
{
	int	i;
	int	j;
	int	value;
	int	other;

	state->split = n;
	i = 0;
	while (i < n)
	{
		cbuf_read_at(a, i, &value);
		state->value[i] = 0;
		j = 0;
		while (j < n)
		{
			cbuf_read_at(a, j++, &other);
			if (other < value)
				state->value[i]++;
		}
		i++;
	}
}

int	seed_is_goal(t_brutestate *state, int n)
{
	int	i;

	if (state->split != n)
		return (0);
	i = 0;
	while (i < n)
	{
		if (state->value[i] != i)
			return (0);
		i++;
	}
	return (1);
}

/* There are (n + 1) * n! distinct split/permutation states. */
int	seed_search_init(t_seed_search *search, circle_buf *a)
{
	int	i;
	int	id;

	ft_memset(search, 0, sizeof(*search));
	search->n = cbuf_len(a);
	search->capacity = 1;
	i = 2;
	while (i <= search->n + 1)
		search->capacity *= i++;
#ifdef BFS_DEBUG
	debug_print_bfs_memory(search->capacity);
#endif
	search->nodes = malloc(sizeof(t_brutenode) * search->capacity);
	search->visited = ft_calloc((search->capacity + 7) / 8, 1);
	if (!search->nodes || !search->visited)
	{
		free(search->nodes);
		free(search->visited);
		return (ERROR);
	}
	seed_rank(&search->nodes[0].state, a, search->n);
	search->nodes[0].parent = -1;
	id = calculate_state_id(&search->nodes[0].state, search->n);
	search->visited[id / 8] |= 1 << (id % 8);
	search->count = 1;
	return (SUCCESS);
}
