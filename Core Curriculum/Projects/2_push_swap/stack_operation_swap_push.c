#include "push_swap.h"

int	sa(cbuf *a)
{
	return (cbuf_swap_top(a));
}

int	sb(cbuf *b)
{
	return (cbuf_swap_top(b));
}

int	ss(cbuf *a, cbuf *b)
{
	return (sa(a) | sb(b));
}

int	pa(cbuf *a, cbuf *b)
{
	int read_number;
	
	if (cbuf_pop_top(b, &read_number))
		return (ERR);
	return (cbuf_push_top(a, read_number));
}

int	pb(cbuf *a, cbuf *b)
{
	int read_number;
	
	if (cbuf_pop_top(a, &read_number))
		return (ERR);
	return (cbuf_push_top(b, read_number));
}