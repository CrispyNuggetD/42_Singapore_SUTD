#include "push_swap.h"

int	ra(soln *x, cbuf *a)
{
	append(x, RA);
	return (cbuf_rotate(a));
}

int	rb(soln *x, cbuf *b)
{
	append(x, RB);
	return (cbuf_rotate(b));
}

int	rr(soln *x, cbuf *a, cbuf *b)
{
	append(x, RR);
	return (cbuf_rotate(a) | cbuf_rotate(b));
}
