#include "push_swap.h"

static int	is_selected(int rank)
{
	if (rank >= 0 && rank <= 8)
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

static int	push_selected(soln *x, cbuf *a, cbuf *b)
{
	int	distance;

	while (cbuf_len(b) < 9)
	{
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
		if (pb(x, a, b) == ERROR)
			return (ERROR);
	}
	return (SUCCESS);
}

static int	copy_real_b(cbuf *fake_b, cbuf *real_b)
{
	int	i;
	int	index;

	i = cbuf_len(real_b);
	while (i > 0)
	{
		index = (real_b->read_idx + i - 1) % real_b->capacity;
		if (cbuf_push_top(fake_b, real_b->buf[index]) == ERROR)
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
	if (move == SS)
		return (ss(x, a, b));
	if (move == PA)
		return (pa(x, a, b));
	if (move == PB)
		return (pb(x, a, b));
	if (move == RA)
		return (ra(x, a));
	if (move == RB)
		return (rb(x, b));
	if (move == RR)
		return (rr(x, a, b));
	if (move == RRA)
		return (rra(x, a));
	if (move == RRB)
		return (rrb(x, b));
	if (move == RRR)
		return (rrr(x, a, b));
	return (ERROR);
}

static int	replay_bfs(soln *real, soln *fake, cbuf *a, cbuf *b)
{
	int	i;
	int	len;

	i = 0;
	len = fake->ans_len[fake->cur];
	while (i < len)
	{
		if (replay_one(real, a, b, fake->ans[fake->cur][i]) == ERROR)
			return (ERROR);
		i++;
	}
	return (SUCCESS);
}

int	debug_hidden_bfs(soln *real, cbuf *a, cbuf *b)
{
	cbuf	fake_a;
	cbuf	fake_b;
	soln	fake;

	printf("\n=== ORIGINAL REAL STACKS ===\n");
	cbuf_print_stacks(a, b);
	if (push_selected(real, a, b) == ERROR)
		return (ERROR);
	printf("\n=== AFTER EXTRACTING RANKS 0..8 ===\n");
	cbuf_print_stacks(a, b);
	fake_a = (cbuf){0};
	fake_b = (cbuf){0};
	fake_a.capacity = a->capacity;
	fake_b.capacity = b->capacity;
	if (copy_real_b(&fake_b, b) == ERROR)
		return (ERROR);
	printf("\n=== BFS IS TOLD ONLY THIS EXISTS ===\n");
	cbuf_print_stacks(&fake_a, &fake_b);
	if (soln_init(&fake, 1, BUBBLE_SORT_MAX_500) == ERROR)
		return (ERROR);
	if (brute_solve(&fake, &fake_a, &fake_b) == ERROR)
		return (ERROR);
	printf("\n=== BFS MOVE COUNT: %d ===\n", fake.ans_len[fake.cur]);
	if (replay_bfs(real, &fake, a, b) == ERROR)
		return (ERROR);
	printf("\n=== REAL STACKS AFTER BFS REPLAY ===\n");
	cbuf_print_stacks(a, b);
	return (SUCCESS);
}