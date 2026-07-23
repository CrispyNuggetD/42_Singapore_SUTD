#include "push_swap.h"
// 20260723(Thu)05:57:24+08:00

void	debug_print_int_array(const int *array, int size)
{
	int	i;

	i = 0;
	while (i < size)
	{
		printf("%d", array[i]);
		if (i < size - 1)
			printf(" ");
		i++;
	}
	printf("\n");
}

static void	cbuf_print_metadata(cbuf *stack, char name)
{
	ft_putchar_fd(name, 2);
	ft_putstr_fd(" [len=", 2);
	ft_putnbr_fd(cbuf_len(stack), 2);
	ft_putstr_fd(" read=", 2);
	ft_putnbr_fd(stack->read_idx, 2);
	ft_putstr_fd(" write=", 2);
	ft_putnbr_fd(stack->write_idx, 2);
	ft_putstr_fd(" capacity=", 2);
	ft_putnbr_fd(stack->capacity, 2);
	ft_putstr_fd("]: ", 2);
}

void	cbuf_print(cbuf *stack, char name)
{
	int	offset;
	int	index;
	int	length;

	length = cbuf_len(stack);
	cbuf_print_metadata(stack, name);
	offset = 0;
	while (offset < length)
	{
		index = (stack->read_idx + offset) % stack->capacity;
		ft_putnbr_fd(stack->buffer[index], 2);
		if (offset + 1 < length)
			ft_putchar_fd(' ', 2);
		offset++;
	}
	ft_putchar_fd('\n', 2);
}

void	cbuf_print_stacks(cbuf *a, cbuf *b)
{
	cbuf_print(a, 'A');
	cbuf_print(b, 'B');
}