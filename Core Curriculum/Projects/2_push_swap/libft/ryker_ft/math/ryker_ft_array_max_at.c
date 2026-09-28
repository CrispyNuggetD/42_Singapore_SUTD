/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ryker_ft_array_max_at.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 18:25:37 by hnah              #+#    #+#             */
/*   Updated: 2026/09/28 18:39:22 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ryker_ft.h"

/*
** Return the first maximum's index and write its value to *max.
** NULL pointers or zero length return -1 without changing *max.
** Require len readable elements, len <= SSIZE_MAX, and max outside array.
*/
ssize_t	ryker_ft_array_max_at(int *array, int *max, size_t len)
{
	size_t	max_index;

	max_index = 0;
	if (!array || !max || len == 0)
		return (-1);
	*max = array[0];
	while (len--)
	{
		if (array[len] >= *max)
		{
			*max = array[len];
			max_index = len;
		}
	}
	return ((ssize_t)max_index);
}
