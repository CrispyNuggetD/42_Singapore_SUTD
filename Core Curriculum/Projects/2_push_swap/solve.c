#include "push_swap.h"

static int	hardcode_check_one(soln *x, cbuf *a)
{
	int	a_size;
	int	pattern;

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

int	solve(soln *x, cbuf *a, cbuf *b, int count)
{
	if (count == 25)
		return (debug_hidden_bfs(x, a, b));
	if (count <= BRUTE_MAX_N)
	{
		printf("sizeof(t_brutestate) = %zu\n", sizeof(t_brutestate));
		printf("sizeof(all t_brutestate) = %zu\n", sizeof(t_brutestate) * BRUTE_TOTAL_N_PLUS_1_FACTORIAL);
		printf("KB sizeof(all t_brutestate) = %zu\n", sizeof(t_brutestate) * BRUTE_TOTAL_N_PLUS_1_FACTORIAL / 1000);
		printf("MB sizeof(all t_brutestate) = %zu\n", sizeof(t_brutestate) * BRUTE_TOTAL_N_PLUS_1_FACTORIAL / 1000000);
		
		if (brute_solve(x, a, b) == ERROR)
			return (ERROR);
	}
	else //check if b is empty and a is less than 3-5 items later.
	{
		if (hardcode_check_one(x, a) == ERROR)
			return (ERROR);
	}
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