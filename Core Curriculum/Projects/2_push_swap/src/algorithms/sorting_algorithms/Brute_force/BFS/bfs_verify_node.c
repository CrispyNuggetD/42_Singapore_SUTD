/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs_verify_node.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/10 20:28:42 by hnah              #+#    #+#             */
/*   Updated: 2026/10/01 18:38:16 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

/*
** Apply BFS moves and check whether a state is sorted with B empty.
**
** The commented helpers at the bottom show the old duplicate-state check:
** scan earlier nodes and compare their arrays. This still works fine, but
** it just gets slower as more states are discovered.
**
** Lehmer IDs and a visited bitset now replace that scan in bfs_search.c:
** O(n * n) ranking plus O(1) lookup, rather than O(total * n) comparisons.
** The BFS algorithm is unchanged; duplicate detection scales better.
**
** If Lehmer indexing feels unfamiliar, read the old helpers first to see
** what it replaces, then bfs_optimiser_lehmer_rank.c and the README for how.
*/

static void	brute_apply_rotate(t_brutestate *state, char move, int n);

/*
** Simulate one encoded move on a compact state; do not record or print it.
** Supports all eleven operations used by the full-input BFS search.
*/
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

/*
** Dispatch rotation codes to compact-state helpers; ignore unrecognised codes.
*/
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

/*
** Accept only split == n: all modelled elements are in A and B is empty.
** Values must be ranks 0 through n-1 in ascending order.
*/
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

/*
** Old duplicate detection: brute_state_exists scanned every discovered node.
** same_brute_state compared its split and all n values with the candidate.
** That cost up to O(total * n) per candidate as the node list grew.
**
** Now calculate_state_id encodes split * n! + the permutation's Lehmer rank.
** For a fixed n and distinct ranks 0..n-1, each state has a unique ID.
** Including split distinguishes identical arrays with different A/B boundaries.
** bfs_search.c computes this ID, then calls state_was_visited on the bitset.
** An unset bit means a new state: mark_state_visited sets it before enqueueing.
** A set bit means the state was discovered earlier, so the candidate is skipped.
**
** The bit lookup is O(1); our nested-loop Lehmer calculation is O(n * n).
** No scan of previous nodes or pairwise array comparison is needed anymore.
** The helpers below are retained as a commented record of the old approach.
*/

/* static int	same_brute_state(t_brutestate *a, t_brutestate *b, int n)
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

int	brute_state_exists(t_brutestate *temp, t_brutenode *nodes, int total, int n)
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
