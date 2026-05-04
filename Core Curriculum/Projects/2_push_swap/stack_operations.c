#include "push_swap.h"

int sa(cbuf *stack)
{
	return ((stack->write_idx - stack->read_idx + stack->capacity) \
% stack->capacity);
}