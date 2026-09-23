#include "push_swap.h"

int	rra(soln *x, circle_buf *a)
{
	append_move_to_soln(x, RRA);
	return (cbuf_rev_rotate(a));
}

int	rrb(soln *x, circle_buf *b)
{
	append_move_to_soln(x, RRB);
	return (cbuf_rev_rotate(b));
}

int	rrr(soln *x, circle_buf *a, circle_buf *b)
{
	append_move_to_soln(x, RRR);
	return (cbuf_rev_rotate(a) | cbuf_rev_rotate(b));
}