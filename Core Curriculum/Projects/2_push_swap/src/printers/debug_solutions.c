/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_solutions.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 06:01:57 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 19:40:19 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

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

static void	debug_soln_header(const soln *x, circle_buf *a_ori)
{
	if (DEBUG < 1)
		return ;
	ryker_ft_printf_fd(2, "Input sequence (original ranks):\n");
	if (a_ori != NULL)
		cbuf_print(a_ori, 'A');
	else
		ryker_ft_printf_fd(2, "a_ori is NULL\n");
	ryker_ft_printf_fd(2, "\n========== SOLUTION DEBUG ==========\n");
	ryker_ft_printf_fd(2, "ans address     : %p\n", (void *)x->ans);
	ryker_ft_printf_fd(2, "ans_len address : %p\n", (void *)x->ans_len);
	ryker_ft_printf_fd(2, "current solution: %d\n", x->cur);
	ryker_ft_printf_fd(2, "current step    : %d\n", x->step);
}

static void	debug_encoded(const soln *x, int index)
{
	int	i;

	if (DEBUG < 1)
		return ;
	ryker_ft_printf_fd(2, "\nSolution [%d]\n", index);
	ryker_ft_printf_fd(2, "Stored length: %d\n", x->ans_len[index]);
	ryker_ft_printf_fd(2, "Encoded      : ");
	i = 0;
	while (i < x->ans_len[index])
		ryker_ft_printf_fd(2, "%c", x->ans[index][i++]);
	ryker_ft_printf_fd(2, "\nDecoded moves:\n");
}

static void	debug_decoded(const soln *x, int index)
{
	int	i;

	if (DEBUG < 2)
		return ;
	i = 0;
	while (i < x->ans_len[index])
	{
		ryker_ft_printf_fd(2, "  Step %d: %s [%c]\n", i + 1,
			move_name(x->ans[index][i]), x->ans[index][i]);
		i++;
	}
}

void	debug_print_soln(const soln *x, circle_buf *a_ori)
{
	int	index;

	if (DEBUG < 1)
		return ;
	if (x == NULL)
	{
		ryker_ft_printf_fd(2, "[SOLN DEBUG] x is NULL\n");
		return ;
	}
	debug_soln_header(x, a_ori);
	if (x->ans == NULL || x->ans_len == NULL)
	{
		ryker_ft_printf_fd(2, "Cannot inspect solutions: NULL pointer\n");
		return ;
	}
	index = 0;
	while (index <= x->cur)
	{
		debug_encoded(x, index);
		debug_decoded(x, index++);
	}
	ryker_ft_printf_fd(2, "\n====================================\n\n");
}
