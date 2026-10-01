/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 21:35:38 by hnah              #+#    #+#             */
/*   Updated: 2026/09/30 19:51:34 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

/* Return n! for n in 0..11, or -1 outside that range. */
int	factorial_max_11(int n)
{
	static const int	factorial_table[12] = {
		1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800, 39916800
	};

	if (n < 0 || n > 11)
		return (-1);
	return (factorial_table[n]);
}

/* Count permutations across all A/B splits; return -1 for unsupported n. */
int	bfs_possible_states(int n)
{
	if (n < 0 || n > BRUTE_MAX_N)
		return (-1);
	return (factorial_max_11(n + 1));
}

/*
** Read the visited bit for this encoded state; nonzero means already discovered.
*/
int	state_was_visited(const unsigned char *visited, int state_id)
{
	return (visited[state_id / 8] & (1 << (state_id % 8)));
}

/*
** Set the visited bit so later routes to the same state are skipped.
*/
void	mark_state_visited(unsigned char *visited, int state_id)
{
	visited[state_id / 8] |= (1 << (state_id % 8));
}
