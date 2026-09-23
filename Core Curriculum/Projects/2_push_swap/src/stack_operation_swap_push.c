#include "push_swap.h"

int	sa(soln *x, circle_buf *a)
{
	append_move_to_soln(x, SA);
	return (cbuf_swap_top(a));
}

int	sb(soln *x, circle_buf *b)
{
	append_move_to_soln(x, SB);
	return (cbuf_swap_top(b));
}

int	ss(soln *x, circle_buf *a, circle_buf *b)
{
	append_move_to_soln(x, SS);
	return (cbuf_swap_top(a) | cbuf_swap_top(b));
}

int	pa(soln *x, circle_buf *a, circle_buf *b)
{
	int	read_number;

	append_move_to_soln(x, PA);
	if (cbuf_pop_top(b, &read_number) == ERROR)
		return (ERROR);
	return (cbuf_push_top(a, read_number));
}

int	pb(soln *x, circle_buf *a, circle_buf *b)
{
	int	read_number;

	append_move_to_soln(x, PB);
	if (cbuf_pop_top(a, &read_number) == ERROR)
		return (ERROR);
	return (cbuf_push_top(b, read_number));
}
