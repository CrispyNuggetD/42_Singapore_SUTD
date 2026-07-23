#include "push_swap.h"


static int	hardcode_check(soln *x, cbuf *a, cbuf *b)
{
	(void)b;
	a_size = cbuf_len(a);
	if (a_size == 3)
	{
		pattern = get_order_top_three(x, a);
		if (pattern == 321)
			return (sa(x, a) | rra(x, a));
		else if (pattern == 312)
			return (ra(x, a));
		else if (pattern == 231)
			return (rra(x, a));
		else if (pattern == 213)
			return (sa(x, a));
		else if (pattern == 132)
			return (sa(x, a) | ra(x, a));
	return (SUCCESS);
}

int	solve(soln *x, cbuf *a, cbuf *b)
{
	hardcode_check(x, a, b);
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