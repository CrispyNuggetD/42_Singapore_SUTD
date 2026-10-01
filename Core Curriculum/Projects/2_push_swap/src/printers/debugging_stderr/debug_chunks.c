/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_chunks.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 06:01:57 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 18:18:03 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	debug_chunk_route(int start, int end, int cost, const char *direction)
{
	if (DEBUG < 2)
		return ;
	ryker_ft_printf_fd(2, "CHUNK %d..%d ROUTE cost=%d start=%s ",
		start, end, cost, direction);
}

void	debug_chunk_turn(int turn)
{
	if (DEBUG < 2)
		return ;
	ryker_ft_printf_fd(2, "turn_after=%d\n", turn);
}

void	debug_total_moves(int total)
{
	if (DEBUG < 2)
		return ;
	ryker_ft_printf_fd(2, "TOTAL MOVES: %d\n", total);
}
