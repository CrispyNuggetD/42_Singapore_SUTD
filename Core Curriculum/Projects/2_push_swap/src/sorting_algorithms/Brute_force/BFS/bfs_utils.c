/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 21:35:38 by hnah              #+#    #+#             */
/*   Updated: 2026/08/12 21:36:45 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

// swaps two elements inside the array during BFS
void	brute_swap_at(t_brutestate *state, int a, int b)
{
	int	temp;

	temp = state->value[a];
	state->value[a] = state->value[b];
	state->value[b] = temp;
}

// rotates the elements in the array to the left during BFS
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