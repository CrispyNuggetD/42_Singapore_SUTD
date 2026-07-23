#include "push_swap.h"

int	sa(soln *x, cbuf *a)
{
	append(x, SA);
	return (cbuf_swap_top(a));
}

int	sb(soln *x, cbuf *b)
{
	return (cbuf_swap_top(b));
}

int	ss(soln *x, cbuf *a, cbuf *b)
{
	return (sa(a) | sb(b));
}

int	pa(soln *x, cbuf *a, cbuf *b)
{
	int read_number;
	
	if (cbuf_pop_top(b, &read_number))
		return (ERROR);
	return (cbuf_push_top(a, read_number));
}

int	pb(soln *x, cbuf *a, cbuf *b)
{
	int read_number;
	
	if (cbuf_pop_top(a, &read_number))
		return (ERROR);
	return (cbuf_push_top(b, read_number));
}
