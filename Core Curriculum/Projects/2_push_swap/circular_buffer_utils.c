#include "push_swap.h"

int cbuf_init(cbuf *stack, int size)
{
	stack->buffer = malloc(sizeof(int) * (size + 1));
	if (!stack->buffer)
		return (1);
	stack->capacity = size;
	stack->read_idx = 0;
	stack->write_idx = 0;
	return (0);
}

int	cbuf_len(cbuf *stack)
{
	return ((stack->write_idx - stack->read_idx + stack->capacity) \
% stack->capacity);
}

int	cbuf_is_empty(cbuf *stack)
{
	return (stack->read_idx == stack->write_idx);
}

int	cbuf_is_full(cbuf *stack)
{
	return ((stack->write_idx + 1) % stack->capacity == stack->read_idx);
}
