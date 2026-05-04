#include "push_swap.h"

int	ra(cbuf *a)
{
	return (cbuf_rotate(a));
}

int	rb(cbuf *b)
{
	return (cbuf_rotate(b));
}

int	rr(cbuf *a, cbuf *b)
{
	return (ra(a) | rb(b));
}