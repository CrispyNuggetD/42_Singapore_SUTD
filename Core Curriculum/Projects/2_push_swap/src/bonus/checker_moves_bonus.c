/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker_moves_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/02 01:13:40 by hnah              #+#    #+#             */
/*   Updated: 2026/10/02 01:13:41 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "checker_bonus.h"

/*
 * TODO: Dispatch exact sa/sb/ss/pa/pb/ra/rb/rr/rra/rrb/rrr lines.
 * Include the newline and string terminator in comparisons.
 * Reject blank lines, extra spaces, suffixes and missing newlines.
 * Call existing operations with NULL for their t_soln pointer.
 * Guard pa/pb with cbuf_is_empty on their source: return SUCCESS.
 * Unknown or malformed instruction: return ERROR.
 * Split dispatch into small static helpers if needed for Norm.
 */
int	checker_apply_move(const char *line, t_circle_buf stacks[2])
{
	(void)line;
	(void)stacks;
	return (ERROR);
}
