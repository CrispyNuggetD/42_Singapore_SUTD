/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DO_NOT_SUBMIT_DEBUG_hidden_bfs.c                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 16:44:38 by hnah              #+#    #+#             */
/*   Updated: 2026/09/23 18:05:28 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	g_start;
static int	g_end;

/*
** Copy the top count values of real B into temporary B in the same order.
** Subtract g_start so this contiguous rank chunk becomes local ranks 0..count-1.
*/
static int	copy_active_b(circle_buf *fake_b, circle_buf *real_b, int count)
{
	int	i;
	int	index;

	i = count;
	while (i > 0)
	{
		index = (real_b->read_idx + i - 1) % real_b->capacity;
		if (cbuf_push_top(fake_b,
		real_b->buf[index] - g_start) == ERROR)
	return (ERROR);
		i--;
	}
	return (SUCCESS);
}

/*
** Execute and record one supported move on the real stacks.
** Handles single-stack swaps, pushes and rotations; combined moves return ERROR.
*/
static int	replay_one(soln *x, circle_buf *a, circle_buf *b, char move)
{
	if (move == SA)
		return (sa(x, a));
	if (move == SB)
		return (sb(x, b));
	if (move == PA)
		return (pa(x, a, b));
	if (move == PB)
		return (pb(x, a, b));
	if (move == RA)
		return (ra(x, a));
	if (move == RB)
		return (rb(x, b));
	if (move == RRA)
		return (rra(x, a));
	if (move == RRB)
		return (rrb(x, b));
	return (ERROR);
}

/*
** Replay the temporary solution in order on real stacks, recording each move.
** Stop on the first failed operation.
*/
static int	replay_bfs(soln *real, soln *fake, circle_buf *a, circle_buf *b)
{
	int	i;

	i = 0;
	while (i < fake->ans_len[fake->cur])
	{
		if (replay_one(real, a, b,
				fake->ans[fake->cur][i]) == ERROR)
			return (ERROR);
		i++;
	}
	return (SUCCESS);
}

/*
** Release the buffers of the temporary one-route solution.
*/
static void	free_fake_solution(soln *fake)
{
	free(fake->ans[0]);
	free(fake->ans);
	free(fake->ans_len);
}

/*
** Build empty temporary A and a locally ranked copy of the active B chunk.
** Search for moves, replay them on real stacks, then free the temporary route.
** The current BFS goal leaves this chunk in descending B, with temporary A empty.
*/
static int	solve_active_chunk(soln *real, circle_buf *a, circle_buf *b, int count)
{
	circle_buf	fake_a;
	circle_buf	fake_b;
	soln	fake;

	fake_a = (circle_buf){0};
	fake_b = (circle_buf){0};
	fake_a.capacity = a->capacity;
	fake_b.capacity = b->capacity;
	if (copy_active_b(&fake_b, b, count) == ERROR)
		return (ERROR);
	if (soln_init(&fake, 1, MAX_MOVES_CONSIDERED) == ERROR)
		return (ERROR);
	if (brute_solve(&fake, &fake_a, &fake_b) == ERROR)
	{
		free_fake_solution(&fake);
		return (ERROR);
	}
	if (replay_bfs(real, &fake, a, b) == ERROR)
	{
		free_fake_solution(&fake);
		return (ERROR);
	}
	free_fake_solution(&fake);
	return (SUCCESS);
}

/*
** Experimental chunk driver: extract successive rank intervals from A to B,
** solve each temporary chunk with restricted BFS, and replay its route.
** Print extraction/search move counts. Earlier values are hidden from the search.
** This is not an unrestricted BFS over the entire input or a final A-sort pass.
*/
int	debug_hidden_bfs(soln *real, circle_buf *a, circle_buf *b)
{
	int	total;
	int	chunk;
	int	before;
	int	extract_moves;
	int	bfs_moves;
	int	bfs_run;
	int	total_bfs_runs;

	total = cbuf_len(a);
	g_start = 0;
	bfs_run = 0;
	total_bfs_runs = (total + BRUTE_MAX_N - 1) / BRUTE_MAX_N;
	while (g_start < total)
	{
		chunk = total - g_start;
		if (chunk > BRUTE_MAX_N)
			chunk = BRUTE_MAX_N;
		g_end = g_start + chunk - 1;
		before = real->ans_len[real->cur];
		if (extract_chunk_optimal(real, a, b, g_start, g_end) == ERROR)
			return (ERROR);
		extract_moves = real->ans_len[real->cur] - before;
		bfs_run++;
		debug_bfs_run(bfs_run, total_bfs_runs, g_start, g_end);
		before = real->ans_len[real->cur];
		if (solve_active_chunk(real, a, b, chunk) == ERROR)
			return (ERROR);
		bfs_moves = real->ans_len[real->cur] - before;
		debug_chunk_result(g_start, g_end, extract_moves, bfs_moves);
		g_start += chunk;
	}
	debug_total_moves(real->ans_len[real->cur]);
	return (SUCCESS);
}
