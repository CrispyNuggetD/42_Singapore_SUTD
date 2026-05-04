#include "push_swap.h"

int	rra(cbuf *a)
{
	return (cbuf_rev_rotate(a));
}

int	rrb(cbuf *b)
{
	return (cbuf_rev_rotate(b));
}

int	rrr(cbuf *a, cbuf *b)
{
	return (rra(a) | rrb(b));
}