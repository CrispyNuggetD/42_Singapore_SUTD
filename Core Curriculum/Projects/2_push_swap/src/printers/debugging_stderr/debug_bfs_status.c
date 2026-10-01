/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_bfs_status.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 19:55:45 by hnah              #+#    #+#             */
/*   Updated: 2026/09/30 19:55:45 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

/* Report the input-size limit without creating an answer candidate. */
void	debug_bfs_skipped(int count)
{
	const t_algo_config	*config;

	if (!DEBUG || count <= BRUTE_MAX_N)
		return ;
	config = algorithm_config(ALGO_BFS);
	if (!config)
		return ;
	ryker_ft_printf_fd(2, "algo=%d/%d%s SKIPPED (n=%d exceeds limit=%d)\n",
		ALGO_BFS + 1, ALGO_COUNT, config->name, count, BRUTE_MAX_N);
}

/* Search-space coverage is not an estimate of time until the goal is found. */
void	debug_bfs_progress(int expanded, int discovered, int capacity)
{
	if (!DEBUG)
		return ;
	if (DEBUG == 1)
		ryker_ft_printf_fd(2, "\r\033[2K");
	ryker_ft_printf_fd(2, "A%d/%d BFS expanded=%d/%d seen=%d queued=%d",
		ALGO_BFS + 1, ALGO_COUNT, expanded, capacity,
		discovered, discovered - expanded);
	if (DEBUG >= 2)
		ryker_ft_printf_fd(2, "\n");
}

/* Print before allocating, so even the initial allocation is visible. */
void	debug_bfs_start(int n, int capacity)
{
	if (!DEBUG)
		return ;
	if (DEBUG == 1)
		ryker_ft_printf_fd(2, "\r\033[2K");
	ryker_ft_printf_fd(2, "A%d/%d BFS n=%d capacity=%d allocating...",
		ALGO_BFS + 1, ALGO_COUNT, n, capacity);
	if (DEBUG >= 2)
		ryker_ft_printf_fd(2, "\n");
}

/* A negative move count denotes allocation failure or search failure. */
void	debug_bfs_end(int moves)
{
	const t_algo_config	*config;

	if (!DEBUG)
		return ;
	if (DEBUG == 1)
		ryker_ft_printf_fd(2, "\r\033[2K");
	config = algorithm_config(ALGO_BFS);
	if (!config)
		return ;
	ryker_ft_printf_fd(2, "algo=%d/%d%s", ALGO_BFS + 1,
		ALGO_COUNT, config->name);
	if (moves < 0)
		ryker_ft_printf_fd(2, " FAILED\n");
	else
		ryker_ft_printf_fd(2, " final_moves=%d DONE\n", moves);
}
