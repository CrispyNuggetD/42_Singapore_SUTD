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
		ft_putnbr_fd(stack->buf[index], 2);
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

static const char	*move_name(char move)
{
	if (move == SA)
		return ("sa");
	if (move == SB)
		return ("sb");
	if (move == SS)
		return ("ss");
	if (move == PA)
		return ("pa");
	if (move == PB)
		return ("pb");
	if (move == RA)
		return ("ra");
	if (move == RB)
		return ("rb");
	if (move == RR)
		return ("rr");
	if (move == RRA)
		return ("rra");
	if (move == RRB)
		return ("rrb");
	if (move == RRR)
		return ("rrr");
	return ("UNKNOWN");
}

void	debug_print_soln(const soln *x)
{
	int	solution_index;
	int	move_index;
	int	length;

	if (x == NULL)
	{
		printf("[SOLN DEBUG] x is NULL\n");
		return ;
	}
	printf("\n========== SOLUTION DEBUG ==========\n");
	printf("ans address     : %p\n", (void *)x->ans);
	printf("ans_len address : %p\n", (void *)x->ans_len);
	printf("current solution: %d\n", x->cur);
	printf("current step    : %d\n", x->step);
	if (x->ans == NULL || x->ans_len == NULL)
	{
		printf("Cannot inspect solutions: NULL pointer\n");
		printf("====================================\n");
		return ;
	}
	solution_index = 0;
	while (solution_index <= x->cur)
	{
		length = x->ans_len[solution_index];
		printf("\nSolution [%d]\n", solution_index);
		printf("Stored length: %d\n", length);
		printf("Encoded      : ");
		move_index = 0;
		while (move_index < length)
		{
			printf("%c", x->ans[solution_index][move_index]);
			move_index++;
		}
		printf("\nDecoded moves:\n");
		move_index = 0;
		while (move_index < length)
		{
			printf("  Step %d: %s [%c]\n",
				move_index + 1,
				move_name(x->ans[solution_index][move_index]),
				x->ans[solution_index][move_index]);
			move_index++;
		}
		solution_index++;
	}
	printf("\n====================================\n\n");
}

static int	print_move(char move)
{
	if (move == SA)
		ft_putendl_fd("sa", STDOUT_FILENO);
	else if (move == SB)
		ft_putendl_fd("sb", STDOUT_FILENO);
	else if (move == SS)
		ft_putendl_fd("ss", STDOUT_FILENO);
	else if (move == PA)
		ft_putendl_fd("pa", STDOUT_FILENO);
	else if (move == PB)
		ft_putendl_fd("pb", STDOUT_FILENO);
	else if (move == RA)
		ft_putendl_fd("ra", STDOUT_FILENO);
	else if (move == RB)
		ft_putendl_fd("rb", STDOUT_FILENO);
	else if (move == RR)
		ft_putendl_fd("rr", STDOUT_FILENO);
	else if (move == RRA)
		ft_putendl_fd("rra", STDOUT_FILENO);
	else if (move == RRB)
		ft_putendl_fd("rrb", STDOUT_FILENO);
	else if (move == RRR)
		ft_putendl_fd("rrr", STDOUT_FILENO);
	else
		return (ERROR);
	return (SUCCESS);
}

int	print_first_soln(const soln *x)
{
	int	i;

	if (x == NULL || x->ans == NULL || x->ans_len == NULL)
		return (ERROR);
	i = 0;
	while (i < x->ans_len[0])
	{
		if (print_move(x->ans[0][i]) == ERROR)
			return (ERROR);
		i++;
	}
	return (SUCCESS);
}