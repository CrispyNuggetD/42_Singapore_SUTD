/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   circular_buffer_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:42:07 by hnah              #+#    #+#             */
/*   Updated: 2026/04/23 13:46:11 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

// 20260723(Thu)13:34:11+08:00

int	soln_init(soln *x, const int soln_num, const int steps_limit)
{
	int	cur_soln;

	x->ans = malloc(sizeof(char *) * (soln_num));
	if (!x->ans)
		return (ERROR);
	cur_soln = (int)soln_num;
	while (cur_soln-- > 0)
	{
		x->ans[cur_soln] = malloc(sizeof(char) * steps_limit);
		if (!x->ans[cur_soln])
		{
			while (++cur_soln < soln_num)
				free(x->ans[cur_soln]);
			free(x->ans);
			return (ERROR);
		}
	}
	return (SUCCESS);
}
