/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_tools.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 06:01:57 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 19:33:03 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

// 20260723(Thu)05:57:24+08:00

void	debug_print_int_array(const int *array, int size)
{
	int	i;

	if (DEBUG < 2)
		return ;
	i = 0;
	while (i < size)
	{
		ryker_ft_printf_fd(STDERR_FILENO, "%d", array[i]);
		if (i < size - 1)
			ryker_ft_printf_fd(STDERR_FILENO, " ");
		i++;
	}
	ryker_ft_printf_fd(STDERR_FILENO, "\n");
}

static void	cbuf_print_metadata(circle_buf *stack, char name)
{
	if (DEBUG < 2)
		return ;
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

void	cbuf_print(circle_buf *stack, char name)
{
	int	offset;
	int	index;
	int	length;

	if (DEBUG < 2)
		return ;
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

void	cbuf_print_stacks(circle_buf *a, circle_buf *b)
{
	if (DEBUG < 2)
		return ;
	cbuf_print(a, 'A');
	cbuf_print(b, 'B');
}

/* Planned storage, not measured resident memory. KB/MB use decimal units. */
void	debug_print_bfs_memory(size_t node_count)
{
	size_t	states;
	size_t	nodes;
	size_t	visited;

	if (DEBUG < 2)
		return ;
	states = sizeof(t_brutestate) * node_count;
	nodes = sizeof(t_brutenode) * node_count;
	visited = (node_count + 7) / 8;
	ryker_ft_printf_fd(2, "[BFS MEMORY] capacity: %llu states\n",
		(unsigned long long)node_count);
	ryker_ft_printf_fd(2, "state: %llu bytes; node: %llu bytes\n",
		(unsigned long long) sizeof(t_brutestate),
		(unsigned long long) sizeof(t_brutenode));
	ryker_ft_printf_fd(2, "state payload: %llu bytes / %llu KB / %llu MB\n",
		(unsigned long long)states, (unsigned long long)(states / 1000),
		(unsigned long long)(states / 1000000));
	ryker_ft_printf_fd(2, "node table: %llu bytes / %llu KB / %llu MB\n",
		(unsigned long long)nodes, (unsigned long long)(nodes / 1000),
		(unsigned long long)(nodes / 1000000));
	ryker_ft_printf_fd(2, "visited: %llu bytes; combined: %llu bytes\n",
		(unsigned long long)visited, (unsigned long long)(nodes + visited));
}
