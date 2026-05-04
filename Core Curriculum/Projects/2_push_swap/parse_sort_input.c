#include "push_swap.h"

int parse_input(input *input, char **argv)
{
	input->input_len = 
	input->sorted_input = malloc(sizeof(int) * (size + 1));
	if (!stack->buffer)
		return (ERROR);
	stack->capacity = size;
	stack->read_idx = 0;
	stack->write_idx = 0;
	return (SUCCESS);
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

void	cbuf_free(cbuf *stack)
{
	free(stack->buffer);
	stack->buffer = NULL;
}