/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 15:56:47 by hnah              #+#    #+#             */
/*   Updated: 2025/12/11 16:17:24 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	const unsigned char	*s_move;

	s_move = (const unsigned char *)s;
	while (n > 0)
	{
		if (*s_move == (unsigned char)c)
			return ((void *)s_move);
		s_move++;
		n--;
	}
	return (NULL);
}

// Need to cast return as void*. 
// const char* type is potentially mutable (wrong).
