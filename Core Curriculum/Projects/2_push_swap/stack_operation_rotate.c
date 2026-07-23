#include "push_swap.h"

int	ra(soln *x, cbuf *a)
{
	return (cbuf_rotate(a));
}

int	rb(soln *x, cbuf *b)
{
	return (cbuf_rotate(b));
}

int	rr(soln *x, cbuf *a, cbuf *b)
{
	return (ra(a) | rb(b));
}