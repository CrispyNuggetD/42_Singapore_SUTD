#include "push_swap.h"

static int	g_start;
static int	g_end;

static int	is_selected(int rank)
{
	if (rank >= g_start && rank <= g_end)
		return (1);
	return (0);
}

static int	find_selected_distance(cbuf *a)
{
	int	i;
	int	index;

	i = 0;
	while (i < cbuf_len(a))
	{
		index = (a->read_idx + i) % a->capacity;
		if (is_selected(a->buf[index]))
			return (i);
		i++;
	}
	return (-1);
}

static int	push_one_selected(soln *x, cbuf *a, cbuf *b)
{
	int	distance;

	distance = find_selected_distance(a);
	if (distance < 0)
		return (ERROR);
	if (distance <= cbuf_len(a) / 2)
	{
		while (distance-- > 0)
			if (ra(x, a) == ERROR)
				return (ERROR);
	}
	else
	{
		distance = cbuf_len(a) - distance;
		while (distance-- > 0)
			if (rra(x, a) == ERROR)
				return (ERROR);
	}
	return (pb(x, a, b));
}

static int	push_chunk(soln *x, cbuf *a, cbuf *b, int count)
{
	while (count > 0)
	{
		if (push_one_selected(x, a, b) == ERROR)
			return (ERROR);
		count--;
	}
	return (SUCCESS);
}

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
	printf("\n=== BFS ACTIVE CHUNK ===\n");
	//cbuf_print_stacks(&fake_a, &fake_b);
	if (soln_init(&fake, 1, BUBBLE_SORT_MAX_500) == ERROR)
		return (ERROR);
	if (brute_solve(&fake, &fake_a, &fake_b) == ERROR)
		return (ERROR);
	printf("BFS MOVE COUNT: %d\n", fake.ans_len[fake.cur]);
	if (replay_bfs(real, &fake, a, b) == ERROR)
		return (ERROR);
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
	printf("\n=== ORIGINAL ===\n");
	cbuf_print_stacks(a, b);
	while (g_start < total)
	{
		chunk = total - g_start;
		if (chunk > BRUTE_MAX_N)
			chunk = BRUTE_MAX_N;
		g_end = g_start + chunk - 1;
		printf("\n=== SELECTING %d..%d ===\n", g_start, g_end);
		before = real->ans_len[real->cur];
		if (push_chunk(real, a, b, chunk) == ERROR)
			return (ERROR);
		extract_moves = real->ans_len[real->cur] - before;
		printf("\n=== AFTER EXTRACTION ===\n");
		//cbuf_print_stacks(a, b);
		before = real->ans_len[real->cur];
		if (solve_active_chunk(real, a, b, chunk) == ERROR)
			return (ERROR);
		bfs_moves = real->ans_len[real->cur] - before;
		printf("EXTRACTION MOVES: %d\n", extract_moves);
		printf("BFS MOVES: %d\n", bfs_moves);
		printf("CHUNK TOTAL: %d\n", extract_moves + bfs_moves);
		printf("\n=== AFTER BFS REPLAY ===\n");
		cbuf_print_stacks(a, b);
		g_start += chunk;
	}
	printf("\n=== FINAL REAL STACKS ===\n");
	cbuf_print_stacks(a, b);
	printf("TOTAL MOVES: %d\n", real->ans_len[real->cur]);
	return (SUCCESS);
}