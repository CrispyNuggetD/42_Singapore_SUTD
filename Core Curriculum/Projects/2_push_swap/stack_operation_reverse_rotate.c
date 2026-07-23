#include "push_swap.h"

int	rra(soln *x, cbuf *a)
{
	return (cbuf_rev_rotate(a));
}

int	rrb(soln *x, cbuf *b)
{
	return (cbuf_rev_rotate(b));
}

int	rrr(soln *x, cbuf *a, cbuf *b)
{
	return (rra(a) | rrb(b));
}