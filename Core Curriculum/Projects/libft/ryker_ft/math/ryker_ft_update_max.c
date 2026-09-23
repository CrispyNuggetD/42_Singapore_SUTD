/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ryker_ft_update_max.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 12:13:58 by hnah              #+#    #+#             */
/*   Updated: 2025/12/11 13:50:31 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ryker_ft.h"

/* Return -1 for NULL, 0 if unchanged, 1 if updated. */
int	ryker_ft_update_max(int *dest, int candidate)
{
	if (!dest)
		return (-1);
	if (candidate > *dest)
	{
		*dest = candidate;
		return (1);
	}
	return (0);
}
