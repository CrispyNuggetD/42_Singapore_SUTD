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

/*
** Simulate sa on the A prefix; fewer than two values means no change.
*/
void	brute_sa(t_brutestate *state)
{
	if (state->split >= 2)
		brute_swap_at(state, 0, 1);
}

/*
** Simulate sb on the B suffix; fewer than two values means no change.
*/
void	brute_sb(t_brutestate *state, int n)
{
	if (n - state->split >= 2)
		brute_swap_at(state, state->split, state->split + 1);
}

/*
** Simulate swapping the top two values of each stack independently.
*/
void	brute_ss(t_brutestate *state, int n)
{
	brute_sa(state);
	brute_sb(state, n);
}

/*
** Move B top to A top in value[], then increase split; empty B is a no-op.
*/
void	brute_pa(t_brutestate *state, int n)
{
	if (state->split >= n)
		return ;
	brute_rotate_right(state, 0, state->split);
	state->split++;
}

/*
** Move A top to B top in value[], then decrease split; empty A is a no-op.
*/
void	brute_pb(t_brutestate *state)
{
	if (state->split <= 0)
		return ;
	brute_rotate_left(state, 0, state->split - 1);
	state->split--;
}