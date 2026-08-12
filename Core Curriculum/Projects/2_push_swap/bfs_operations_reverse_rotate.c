/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs_operations_reverse_rotate.c                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 21:35:37 by hnah              #+#    #+#             */
/*   Updated: 2026/08/12 21:38:34 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	brute_rra(t_brutestate *state)
{
	brute_rotate_right(state, 0, state->split - 1);
}

void	brute_rrb(t_brutestate *state, int n)
{
	brute_rotate_right(state, state->split, n - 1);
}

void	brute_rrr(t_brutestate *state, int n)
{
	brute_rra(state);
	brute_rrb(state, n);
}