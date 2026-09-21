#include "push_swap.h"

int	rra(soln *x, cbuf *a)
{
	append(x, RRA);
	return (cbuf_rev_rotate(a));
}

int	rrb(soln *x, cbuf *b)
{
	append(x, RRB);
	return (cbuf_rev_rotate(b));
}

int	rrr(soln *x, cbuf *a, cbuf *b)
{
	append(x, RRR);
	return (cbuf_rev_rotate(a) | cbuf_rev_rotate(b));
}