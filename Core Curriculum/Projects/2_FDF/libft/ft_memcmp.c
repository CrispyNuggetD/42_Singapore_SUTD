/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 16:18:20 by hnah              #+#    #+#             */
/*   Updated: 2025/12/11 16:20:13 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char	*s1_move;
	const unsigned char	*s2_move;

	s1_move = (const unsigned char *)s1;
	s2_move = (const unsigned char *)s2;
	if (n == 0)
		return (0);
	while ((*s1_move == *s2_move) && n > 1)
	{
		s1_move++;
		s2_move++;
		n--;
	}
	return (*s1_move - *s2_move);
}
