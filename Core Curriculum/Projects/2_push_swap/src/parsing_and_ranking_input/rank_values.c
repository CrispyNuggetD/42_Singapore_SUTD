/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rank_values.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 06:01:57 by hnah              #+#    #+#             */
/*   Updated: 2026/10/01 18:52:21 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

/*
** Rank each value by counting smaller values; input values stay unchanged.
** Requires distinct values and a separate output array of at least count ints.
** Produces ranks 0..count-1 in O(count * count) time.
*/

int	rank_values(const int count, const int *values, int *ranks)
{
	int	i;
	int	j;
	int	pos;

	i = 0;
	{
		while (i < count)
		{
			pos = 0;
			j = 0;
			while (j < count)
			{
				if (values[i] > values[j++])
					pos++;
			}
			ranks[i++] = pos;
		}
		return (SUCCESS);
	}
	return (ERROR);
}
