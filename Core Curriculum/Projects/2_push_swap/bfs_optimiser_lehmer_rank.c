/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs_optimiser_lehmer_rank.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 19:14:49 by hnah              #+#    #+#             */
/*   Updated: 2026/08/19 20:12:03 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static const int	g_factorial[11] = {
	1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800
};

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
		rank += smaller_right_count * g_factorial[n - 1 - i];
		i++;
	}
	return (rank);
}
int	calculate_state_id(t_brutestate *a, int n)
{
	return (a->split * g_factorial[n] + calculate_lehmer_rank(a, n));
}
