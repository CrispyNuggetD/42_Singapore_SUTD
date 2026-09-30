/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   greedy_opening.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 15:45:36 by hnah              #+#    #+#             */
/*   Updated: 2026/09/30 15:45:36 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"

/*
** PSEUDOCODE SCAFFOLD: not compiled or listed in the Makefile yet.
** Replace the English steps with C; split helpers later for Norm.
** Keep the ordinary choose_plan() free of opening-retry logic.
*/

/*
** Score one forced opening on copied stacks, followed by the normal finish.
** Return cost >= 0, or -1 on error. Never change the caller's stacks/recorder.
** Include the first insertion and final alignment; exclude the shared seed.
** Trials must not update the real pass's debug counters: add quiet trial
** execution before wiring this into the solver. Do not assign to DEBUG.
*/
static int	score_opening(circle_buf *a, circle_buf *b,
	int depth, const t_greedy_plan *first)
{
	circle_buf	copies[2];
	soln		trial;
	int			cost;

	copy *a and *b into copies;
	allocate trial with soln_init(&trial, 1, MAX_MOVES_CONSIDERED);
	if allocation failed:
		free any partially allocated trial storage;
		return -1;
	trial.cur = 0;
	trial.step = 0;
	trial.ans_len[0] = 0;

	execute first on copies using greedy_execute_plan(&trial, ...);
	finish copies using greedy_insert_all(&trial, ..., depth);
	align copies A using rot_a_min_to_top(&trial, ...);
	if ANY of these calls failed:
		free trial.ans and trial.ans_len;
		return -1;

	cost = trial.step;
	free trial.ans and trial.ans_len;
	return cost;
}

/* Each iteration tries a different B index from the SAME prepared state. */
int	greedy_choose_opening(circle_buf *a, circle_buf *b,
	int depth, t_greedy_plan *best_first)
{
	t_greedy_plan	candidate;
	int				index;
	int				score;
	int				best_score;

	validate pointers, depth in 0..LOOKAHEAD_DEPTH, and nonempty B;
	best_score = -1;
	index = 0;
	while (index < cbuf_len(b))
	{
		build candidate using greedy_plan_candidate(a, b, index, &candidate);
		if it returned ERROR:
			return ERROR;
		score = score_opening(a, b, depth, &candidate);
		if score < 0:
			return ERROR;
		if best_score < 0 OR score < best_score:
			best_score = score;
			*best_first = candidate;
		index++;
	}
	return SUCCESS;
}

/*
** WIRING LATER, in the OUTER algorithm (never inside choose_plan):
** 1. Prepare the seed once, recording its real moves.
** 2. If B is nonempty, choose the opening on copies using the helper above.
** 3. Start real progress with B's length BEFORE the forced first insertion.
** 4. Execute the winning first plan on real stacks; count one real insertion.
** 5. Finish with ordinary greedy_insert_all at the SAME depth and execution
**    limit used by the trials, then perform final alignment.
** 6. Report final moves including preparation.
**
** No x[500] needed. One temporary recorder is enough; this first draft
** allocates it per trial for clarity. Reusing its allocation can come later.
** Check move-buffer capacity before recording: append_move_to_soln currently
** has no bounds check. Never let a trial write beyond its allocated buffer.
** For an empty B after preparation, skip selection and only align A.
*/
