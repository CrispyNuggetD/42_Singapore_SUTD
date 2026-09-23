#include "push_swap.h"

int	ra(soln *x, circle_buf *a)
{
	append_move_to_soln(x, RA);
	return (cbuf_rotate(a));
}

int	rb(soln *x, circle_buf *b)
{
	append_move_to_soln(x, RB);
	return (cbuf_rotate(b));
}

int	rr(soln *x, circle_buf *a, circle_buf *b)
{
	append_move_to_soln(x, RR);
	return (cbuf_rotate(a) | cbuf_rotate(b));
}
