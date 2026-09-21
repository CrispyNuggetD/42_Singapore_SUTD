/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs_operations_swap_push.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 21:35:29 by hnah              #+#    #+#             */
/*   Updated: 2026/08/12 21:38:34 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	brute_sa(t_brutestate *state)
{
	if (state->split >= 2)
		brute_swap_at(state, 0, 1);
}

void	brute_sb(t_brutestate *state, int n)
{
	if (n - state->split >= 2)
		brute_swap_at(state, state->split, state->split + 1);
}

void	brute_ss(t_brutestate *state, int n)
{
	brute_sa(state);
	brute_sb(state, n);
}

void	brute_pa(t_brutestate *state, int n)
{
	if (state->split >= n)
		return ;
	brute_rotate_right(state, 0, state->split);
	state->split++;
}

void	brute_pb(t_brutestate *state)
{
	if (state->split <= 0)
		return ;
	brute_rotate_left(state, 0, state->split - 1);
	state->split--;
}