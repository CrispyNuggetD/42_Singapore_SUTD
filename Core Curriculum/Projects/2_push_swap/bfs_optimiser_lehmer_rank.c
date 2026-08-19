/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs_optimiser_lehmer_rank.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 19:14:49 by hnah              #+#    #+#             */
/*   Updated: 2026/08/19 19:23:29 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	factorial(int n)
{
	if (n <= 1)
		return (1);
	return (n * factorial(n - 1));
}

int	calculate_lehmer_rank(t_brutestate *state, int n)
{
	int	rank;
	int	i;
	int	temp_right;
	int	smaller_right_count;

	rank = 0;
	i = 0;
	while (i < n)
	{
		smaller_right_count = 0;
		temp_right = i + 1;
		while (temp_right < n)
		{
			if (state->value[temp_right] < state->value[i])
				smaller_right_count++;
			temp_right++;
		}
		rank += smaller_right_count * factorial(n - 1 - i);
		i++;
	}
	return (rank);
}

static int	same_lehmer_rank(t_brutestate *a, t_brutestate *b, int n)
{
	return (calculate_lehmer_rank(a, n) == calculate_lehmer_rank(b, n));
}

int	brute_state_exists(t_brutestate *temp, t_brutenode *nodes, int total, int n)
{
	int	j;

	j = 0;
	while (j < total)
	{
		if (same_lehmer_rank(temp, &nodes[j].state, n))
			return (1);
		j++;
	}
	return (0);
}