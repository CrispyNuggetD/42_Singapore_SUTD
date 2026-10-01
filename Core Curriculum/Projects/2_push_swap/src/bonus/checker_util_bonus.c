/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker_util_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/02 01:13:40 by hnah              #+#    #+#             */
/*   Updated: 2026/10/02 02:51:59 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "checker_bonus.h"

/*
 * Read stdin until EOF and apply each instruction in order.
 * Free each returned line and clean up the reader on every exit path.
 * Invalid instructions or read failures return ERROR; EOF returns SUCCESS.
 */
int	checker_read_apply_moves(t_circle_buf stacks[2])
{
	t_gnl_info		gnl;
	t_gnl_result	result;
	char			*line;

	ryker_ft_gnl_init(&gnl, STDIN_FILENO);
	result = ryker_ft_get_next_line(&gnl, &line);
	while (result == GNL_LINE)
	{
		if (checker_apply_move(line, stacks) == ERROR)
		{
			free(line);
			ryker_ft_gnl_cleanup(&gnl);
			return (ERROR);
		}
		free(line);
		result = ryker_ft_get_next_line(&gnl, &line);
	}
	free(line);
	ryker_ft_gnl_cleanup(&gnl);
	if (result == GNL_ERROR)
		return (ERROR);
	return (SUCCESS);
}

/*
 * pa takes from B; pb takes from A. An empty source makes either move a no-op.
 * The existing wrappers return ERROR when popping an empty stack, so the
 * checker guards the source and returns SUCCESS before calling the wrapper.
 * In the instruction-name array below, pa is index 3 and pb is index 4.
 */
int	checker_apply_move(const char *line, t_circle_buf stacks[2])
{
	static t_single_move const	single[11] = {
		sa, sb, NULL, NULL, NULL, ra, rb, NULL, rra, rrb, NULL};
	static t_double_move const	both[11] = {
		NULL, NULL, ss, pa, pb, NULL, NULL, rr, NULL, NULL, rrr};
	static const char			*moves[11] = {"sa\n", "sb\n", "ss\n", "pa\n",
		"pb\n", "ra\n", "rb\n", "rr\n", "rra\n", "rrb\n", "rrr\n"};
	int							i;

	i = 0;
	while (i < 11)
	{
		if (ft_strncmp(line, moves[i], ft_strlen(moves[i]) + 1) == 0)
		{
			if ((i == 3 && cbuf_is_empty(&stacks[B]))
				|| (i == 4 && cbuf_is_empty(&stacks[A])))
				return (SUCCESS);
			if (both[i])
				return (both[i](NULL, &stacks[A], &stacks[B]));
			if (i == 1 || i == 6 || i == 9)
				return (single[i](NULL, &stacks[B]));
			return (single[i](NULL, &stacks[A]));
		}
		i++;
	}
	return (ERROR);
}
