/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs_operations_rotate.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 21:35:36 by hnah              #+#    #+#             */
/*   Updated: 2026/08/12 21:38:34 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

/*
** Simulate ra: move the first value of the A prefix to its end.
*/
void	brute_ra(t_brutestate *state)
{
	brute_rotate_left(state, 0, state->split - 1);
}

/*
** Simulate rb: move the first value of the B suffix to its end.
*/
void	brute_rb(t_brutestate *state, int n)
{
	brute_rotate_left(state, state->split, n - 1);
}

/*
** Simulate forward rotation of both stacks independently.
*/
void	brute_rr(t_brutestate *state, int n)
{
	brute_ra(state);
	brute_rb(state, n);
}