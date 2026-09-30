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



int	factorial_max_11(int n)
{
	static const int	factorial_table[12] = {
	1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800, 39916800
	};
	
	if (n < 0 || n > 11)
		return (-1);
	return (factorial_table[n]);
}

int	bfs_possible_states(int n)
{
	if (n < 0 || n > BRUTE_MAX_N)
		return (-1);
	return (factorial_max_11(n + 1));
}


// swaps two elements inside the array during BFS
/*
** Swap two physical entries in the compact state array without changing split.
*/
void	brute_swap_at(t_brutestate *state, int a, int b)
{
	int	temp;

	temp = state->value[a];
	state->value[a] = state->value[b];
	state->value[b] = temp;
}

// rotates the elements in the array to the left during BFS
/*
** Rotate inclusive array range [start, end] left: its first value becomes last.
** Empty or one-element ranges are unchanged; split is unchanged.
*/
void	brute_rotate_left(t_brutestate *state, int start, int end)
{
	int	temp;
	int	i;

	if (start >= end)
		return ;
	temp = state->value[start];
	i = start;
	while (i < end)
	{
		state->value[i] = state->value[i + 1];
		i++;
	}
	state->value[end] = temp;
}

// rotates the elements in the array to the right during BFS
/*
** Rotate inclusive array range [start, end] right: its last value becomes first.
** Empty or one-element ranges are unchanged; split is unchanged.
*/
void	brute_rotate_right(t_brutestate *state, int start, int end)
{
	int	temp;
	int	i;

	if (start >= end)
		return ;
	temp = state->value[end];
	i = end;
	while (i > start)
	{
		state->value[i] = state->value[i - 1];
		i--;
	}
	state->value[start] = temp;
}