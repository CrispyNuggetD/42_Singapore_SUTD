#include "push_swap.h"

int	sa(soln *x, circle_buf *a)
{
	append(x, SA);
	return (cbuf_swap_top(a));
}

int	sb(soln *x, circle_buf *b)
{
	append(x, SB);
	return (cbuf_swap_top(b));
}

int	ss(soln *x, circle_buf *a, circle_buf *b)
{
	append(x, SS);
	return (cbuf_swap_top(a) | cbuf_swap_top(b));
}

int	pa(soln *x, circle_buf *a, circle_buf *b)
{
	int	read_number;

	append(x, PA);
	if (cbuf_pop_top(b, &read_number) == ERROR)
		return (ERROR);
	return (cbuf_push_top(a, read_number));
}

int	pb(soln *x, circle_buf *a, circle_buf *b)
{
	int	read_number;

	append(x, PB);
	if (cbuf_pop_top(a, &read_number) == ERROR)
		return (ERROR);
	return (cbuf_push_top(b, read_number));
}
