#include "push_swap.h"

static int	g_start;
static int	g_end;

static int	copy_active_b(cbuf *fake_b, cbuf *real_b, int count)
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

static int	replay_one(soln *x, cbuf *a, cbuf *b, char move)
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

static int	replay_bfs(soln *real, soln *fake, cbuf *a, cbuf *b)
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

static void	free_fake_solution(soln *fake)
{
	free(fake->ans[0]);
	free(fake->ans);
	free(fake->ans_len);
}

static int	solve_active_chunk(soln *real, cbuf *a, cbuf *b, int count)
{
	cbuf	fake_a;
	cbuf	fake_b;
	soln	fake;

	fake_a = (cbuf){0};
	fake_b = (cbuf){0};
	fake_a.capacity = a->capacity;
	fake_b.capacity = b->capacity;
	if (copy_active_b(&fake_b, b, count) == ERROR)
		return (ERROR);
	if (soln_init(&fake, 1, BUBBLE_SORT_MAX_500) == ERROR)
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

int	debug_hidden_bfs(soln *real, cbuf *a, cbuf *b)
{
	int	total;
	int	chunk;
	int	before;
	int	extract_moves;
	int	bfs_moves;

	total = cbuf_len(a);
	g_start = 0;
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
		before = real->ans_len[real->cur];
		if (solve_active_chunk(real, a, b, chunk) == ERROR)
			return (ERROR);
		bfs_moves = real->ans_len[real->cur] - before;
		printf("CHUNK %d..%d extraction=%d bfs=%d total=%d\n", g_start,
			g_end, extract_moves, bfs_moves, extract_moves + bfs_moves);
		g_start += chunk;
	}
	printf("TOTAL MOVES: %d\n", real->ans_len[real->cur]);
	return (SUCCESS);
}
