/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   greedy_prepare.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:11:30 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 17:18:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"

static int	prepare_lis(soln *x, circle_buf *a, circle_buf *b)
{
	char	keep[500];
	int		count;
	int		i;
	int		outcome;

	ft_memset(keep, 0, sizeof(keep));
	count = cbuf_len(a);
	if (cbuf_lis(a, keep) == ERROR)
		return (ERROR);
	i = 0;
	while (i < count)
	{
		if (keep[i++] == 0)
			outcome = pb(x, a, b);
		else
			outcome = ra(x, a);
		if (outcome == ERROR)
			return (ERROR);
	}
	return (SUCCESS);
}

static int	prepare_three(soln *x, circle_buf *a)
{
	int	first;
	int	second;

	if (cbuf_len(a) == 2)
	{
		cbuf_read_at(a, 0, &first);
		cbuf_read_at(a, 1, &second);
		if (first > second)
			return (sa(x, a));
	}
	return (hardcode_three(x, a));
}

/* B must start empty. */
int	greedy_prepare(soln *x, circle_buf *a, circle_buf *b,
	t_seed_mode mode)
{
	if (cbuf_len(b) != 0 || mode < SEED_THREE || mode >= SEED_COUNT)
		return (ERROR);
	if (mode == SEED_LIS)
		return (prepare_lis(x, a, b));
	while (cbuf_len(a) > 3)
	{
		if (pb(x, a, b) == ERROR)
			return (ERROR);
	}
	return (prepare_three(x, a));
}
