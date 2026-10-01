/* Archived hidden-A helpers; reference only, excluded from the build. */
/* The current full-input BFS models every element and needs no wall guard. */

#include "push_swap.h"

/*
** Reject moves that would involve the unmodelled tail of real A.
** Block sa with fewer than two visible values and A rotations when A is visible.
** This guard covers the six moves currently tried by bfs_find_goal.
*/
static int	move_hits_hidden_a(t_brutestate *state, char move)
{
	if (state->split < 2 && move == SA)
		return (1);
	if (state->split > 0 && (move == RA || move == RRA))
		return (1);
	return (0);
}

/*
** Apply a move to a search-state copy only if the hidden-A guard permits it.
** Return 1 when permitted, 0 when blocked; a permitted move may still be a no-op.
*/
static int	brute_apply_wall_move(t_brutestate *state, char move, int n)
{
	if (move_hits_hidden_a(state, move))
		return (0);
	brute_apply_move(state, move, n);
	return (1);
}

