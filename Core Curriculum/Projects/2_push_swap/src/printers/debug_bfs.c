/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_bfs.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 06:01:57 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 18:18:03 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	debug_bfs_alloc(size_t bytes)
{
	if (DEBUG < 2)
		return ;
	ryker_ft_printf_fd(2, "BFS ALLOCATING: %llu bytes\n",
		(unsigned long long)bytes);
}

void	debug_print_message(const char *message)
{
	if (DEBUG < 2)
		return ;
	ryker_ft_printf_fd(2, "%s\n", message);
}

void	debug_bfs_run(int run, int total, int start, int end)
{
	if (DEBUG < 2)
		return ;
	ryker_ft_printf_fd(2, "BFS RUN: %d/%d, remaining after this=%d, ",
		run, total, total - run);
	ryker_ft_printf_fd(2, "chunk=%d..%d\n", start, end);
}

void	debug_chunk_result(int start, int end, int extraction, int bfs)
{
	if (DEBUG < 2)
		return ;
	ryker_ft_printf_fd(2, "CHUNK %d..%d extraction=%d bfs=%d total=%d\n",
		start, end, extraction, bfs, extraction + bfs);
}
