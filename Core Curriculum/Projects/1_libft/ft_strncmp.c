/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 14:48:11 by hnah              #+#    #+#             */
/*   Updated: 2025/12/11 15:55:27 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	const unsigned char	*s1_move;
	const unsigned char	*s2_move;

	s1_move = (const unsigned char *)s1;
	s2_move = (const unsigned char *)s2;
	if (n == 0)
		return (0);
	while (*s1_move && *s2_move && (*s1_move == *s2_move) && n > 1)
	{
		s1_move++;
		s2_move++;
		n--;
	}
	return (*s1_move - *s2_move);
}

// abcX and abcY,
// n-- doesn't work as will compare X and Y when n = 3 (pointer already moved)
