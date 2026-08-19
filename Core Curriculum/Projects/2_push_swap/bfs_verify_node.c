/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs_verify_node.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/10 20:28:42 by hnah              #+#    #+#             */
/*   Updated: 2026/08/19 19:22:46 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	brute_apply_rotate(t_brutestate *state, char move, int n);

void	brute_apply_move(t_brutestate *state, char move, int n)
{
	if (move == SA)
		brute_sa(state);
	else if (move == SB)
		brute_sb(state, n);
	else if (move == SS)
		brute_ss(state, n);
	else if (move == PA)
		brute_pa(state, n);
	else if (move == PB)
		brute_pb(state);
	else
		brute_apply_rotate(state, move, n);
}

static void	brute_apply_rotate(t_brutestate *state, char move, int n)
{
	if (move == RA)
		brute_ra(state);
	else if (move == RB)
		brute_rb(state, n);
	else if (move == RR)
		brute_rr(state, n);
	else if (move == RRA)
		brute_rra(state);
	else if (move == RRB)
		brute_rrb(state, n);
	else if (move == RRR)
		brute_rrr(state, n);
}

// old: same_brute_state() is now replaced by Lehmer rank optimised versions in bfs_optimiser_lehmer_rank.c
static int	same_brute_state(t_brutestate *a, t_brutestate *b, int n)
{
	int	i;

	if (a->split != b->split)
		return (0);
	i = 0;
	while (i < n)
	{
		if (a->value[i] != b->value[i])
			return (0);
		i++;
	}
	return (1);
}

/* int	brute_state_exists(t_brutestate *temp, t_brutenode *nodes, int total, int n)
{
	int	j;

	j = 0;
	while (j < total)
	{
		if (same_brute_state(temp, &nodes[j].state, n))
			return (1);
		j++;
	}
	return (0);
} */

int	is_brute_goal(t_brutestate *state, int n)
{
	int	i;

	if (state->split != n)
		return (0);
	i = 0;
	while (i < n)
	{
		if (state->value[i] != i)
			return (0);
		i++;
	}
	return (1);
}
