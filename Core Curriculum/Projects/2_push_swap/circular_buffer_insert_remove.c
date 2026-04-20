#include "push_swap.h"

int cbuf_push_top(cbuf *stack, int number)
{
	if (cbuf_is_full(stack))
		return (1);
	stack->read_idx = (stack->read_idx - 1 + stack->capacity) \
% stack->capacity;
	stack->buffer[stack->read_idx] = number;
	return (0);
}

int cbuf_push_bottom(cbuf *stack, int number)
{
	if (cbuf_is_full(stack))
		return (1);
	stack->buffer[stack->write_idx] = number;
	stack->write_idx = (stack->write_idx + 1) % stack->capacity;
	return (0);
}

int cbuf_pop_top(cbuf *stack, int *read_number)
{
	if (cbuf_is_empty(stack))
		return (1);
	*read_number = stack->buffer[stack->read_idx];
	stack->read_idx = (stack->read_idx + 1) % stack->capacity;
	return (0);
}
