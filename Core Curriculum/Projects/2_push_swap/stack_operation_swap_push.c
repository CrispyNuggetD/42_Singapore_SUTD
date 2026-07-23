#include "push_swap.h"

int	sa(soln *x, cbuf *a)
{
	append(x, SA);
	return (cbuf_swap_top(a));
}

int	sb(soln *x, cbuf *b)
{
	append(x, SB);
	return (cbuf_swap_top(b));
}

int	ss(soln *x, cbuf *a, cbuf *b)
{
	append(x, SS);
	return (cbuf_swap(a) | cbuf_swap(b));
}

int	pa(soln *x, cbuf *a, cbuf *b)
{
	append(x, PA);
	int read_number;
	
	if (cbuf_pop_top(b, &read_number))
		return (ERROR);
	return (cbuf_push_top(a, read_number));
}

int	pb(soln *x, cbuf *a, cbuf *b)
{
	append(x, PB);
	int read_number;
	
	if (cbuf_pop_top(a, &read_number))
		return (ERROR);
	return (cbuf_push_top(b, read_number));
}
