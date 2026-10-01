/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   solution_printers.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 06:01:57 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 18:21:42 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

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

int	print_best_soln(const t_soln *x)
{
	int	i;
	int	best_algo;

	if (x == NULL || x->ans == NULL || x->ans_len == NULL || x->cur < 0)
		return (ERROR);
	i = 1;
	best_algo = 0;
	while (i <= x->cur)
	{
		if (x->ans_len[i] < x->ans_len[best_algo])
			best_algo = i;
		i++;
	}
	i = 0;
	while (i < x->ans_len[best_algo])
	{
		if (print_move(x->ans[best_algo][i]) == ERROR)
			return (ERROR);
		i++;
	}
	return (SUCCESS);
}
