#include "push_swap.h"

int	cbuf_rotate(cbuf *stack)
{
	if (cbuf_len(stack) < 2)
		return (ERROR);
	stack->read_idx = (stack->read_idx + 1) % stack->capacity;
	stack->write_idx = (stack->write_idx + 1) % stack->capacity;
	return (SUCCESS);
}

int	cbuf_rev_rotate(cbuf *stack)
{
	if (cbuf_len(stack) < 2)
		return (ERROR);
	stack->read_idx = (stack->read_idx - 1 + stack->capacity) \
% stack->capacity;
	stack->write_idx = (stack->write_idx - 1 + stack->capacity) \
% stack->capacity;
	return (SUCCESS);
}

int cbuf_swap_top(cbuf *stack)
{
	int	first_idx;
	int	second_idx;
	int	temp_storage;

	if (cbuf_len(stack) < 2)
		return (ERROR);
	first_idx = stack->read_idx;
	second_idx = (stack->read_idx + 1) % stack->capacity;
	temp_storage = stack->buffer[first_idx];
	stack->buffer[first_idx] = stack->buffer[second_idx];
	stack->buffer[second_idx] = temp_storage;
	return (SUCCESS);
}
