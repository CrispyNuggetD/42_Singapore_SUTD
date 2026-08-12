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

void	brute_ra(t_brutestate *state)
{
	brute_rotate_left(state, 0, state->split - 1);
}

void	brute_rb(t_brutestate *state, int n)
{
	brute_rotate_left(state, state->split, n - 1);
}

void	brute_rr(t_brutestate *state, int n)
{
	brute_ra(state);
	brute_rb(state, n);
}