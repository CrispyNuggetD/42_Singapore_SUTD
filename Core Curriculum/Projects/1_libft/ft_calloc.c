/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 19:08:05 by hnah              #+#    #+#             */
/*   Updated: 2025/12/12 16:45:14 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	unsigned char	*mallocd;
	unsigned char	*mallocd_moving;
	size_t			total;

	if (size == 0 || nmemb == 0)
		return (malloc(1)));
	if (size != 0 && nmemb > SIZE_MAX / size)
		return (NULL);
	total = nmemb * size;
	mallocd = malloc(total);
	if (!mallocd)
		return (NULL);
	mallocd_moving = mallocd;
	while (total--)
	{
		*mallocd_moving = 0;
		mallocd_moving++;
	}
	return ((void *)mallocd);
}

// I found out rather late that I can have limit.h apparently as they're
// not functions. 
//
// say alloc 1 * 4(int) = 4
//
// total = 4-- (4 - 1 = 3)
// mallocd first index[0] = 0, shift ptr by ONE. first is set
// Runs 4 times.
