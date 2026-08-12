#include "push_swap.h"

static int	hardcode_check(soln *x, cbuf *a, cbuf *b)
{
	int	a_size;
	int	pattern;

	(void)b;
	a_size = cbuf_len(a);
	if (a_size == 3)
	{
		pattern = get_order_top_three(a);
		if (pattern == 321)
		{
			if (sa(x, a) == ERROR)
				return (ERROR);
			return (rra(x, a));
		}
		else if (pattern == 312)
			return (ra(x, a));
		else if (pattern == 231)
			return (rra(x, a));
		else if (pattern == 213)
			return (sa(x, a));
		else if (pattern == 132)
		{
			if (sa(x, a) == ERROR)
				return (ERROR);
			return (ra(x, a));
		}
	}
	return (SUCCESS);
}

void	gen_brute_state(t_brutestate *state, cbuf *a, cbuf *b)
{
	int	offset;
	int	index;
	int	pos;
	int	a_len;
	int	b_len;

	a_len = cbuf_len(a);
	b_len = cbuf_len(b);
	state->split = a_len;
	pos = 0;
	offset = 0;
	while (offset < a_len)
	{
		index = (a->read_idx + offset) % a->capacity;
		state->value[pos] = a->buf[index];
		pos++;
		offset++;
	}
	offset = 0;
	while (offset < b_len)
	{
		index = (b->read_idx + offset) % b->capacity;
		state->value[pos] = b->buf[index];
		pos++;
		offset++;
	}
}

void	brute_apply_move(t_brutestate *state, char move)
{
	/* Implementation for applying move to brute state */
}

static int	bfs(soln *x, cbuf *a, cbuf *b)
{
	t_brutenode	nodes[40320];
	t_brutestate	temp;
	int			i;
	int			total;
	int			move_to_try;
	char		moves[11] = {SA, SB, SS, PA, PB, RA, RB, RR, RRA, RRB, RRR};

	(void)x;
	gen_brute_state(&nodes[0].state, a, b);
	nodes[0].parent = -1;
	nodes[0].move = 0;
	i = 0;
	total = 1;
	while (i < total)
	{
		move_to_try = 0;
		while (move_to_try < 11)
		{
			try_move = moves[move_to_try];
			brute_apply_move(&temp, moves[move_to_try]);
			temp = nodes[i].state;
			/* apply one move to temp */
			/* later: check duplicate / Lehmer */
			/* later: append into nodes[total] */
			move_to_try++;
		}
		i++;
	}
	return (ERROR);
}

int	solve(soln *x, cbuf *a, cbuf *b)
{
	if (hardcode_check(x, a, b) == ERROR)
		return (ERROR);
	return (SUCCESS);
}

/*
	if (a_size == -1)
		a_size = cbuf_len(a);
	if (recur == -1)
		recur = a_size;
	if ((recur == 2 || a_size == 2) && (larger_top(x, a)))
		return (sa(x, a));
*/