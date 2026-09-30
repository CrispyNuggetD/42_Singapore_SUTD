/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs_optimiser_lehmer_rank.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 19:14:49 by hnah              #+#    #+#             */
/*   Updated: 2026/09/30 19:45:45 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

/*
** Encode the permutation by counting smaller values to the right of each entry.
** Factorial weights produce a permutation rank from 0 to n! - 1.
*/
static int	calculate_lehmer_rank(t_brutestate *state, int n)
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
		rank += smaller_right_count * factorial_max_11(n - 1 - i);
		i++;
	}
	return (rank);
}
/*
** Combine the A/B split and permutation rank into a visited-table index.
** ID = split * n! + permutation rank; requires n distinct ranks in the state.
*/
int	calculate_state_id(t_brutestate *a, int n)
{
	return (a->split * factorial_max_11(n) + calculate_lehmer_rank(a, n));
}
