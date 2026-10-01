/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs_array_helpers.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 21:35:38 by hnah              #+#    #+#             */
/*   Updated: 2026/09/30 19:51:34 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

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
