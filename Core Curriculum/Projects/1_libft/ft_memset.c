/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 17:09:17 by hnah              #+#    #+#             */
/*   Updated: 2025/12/11 12:54:10 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *s, int c, size_t n)
{
	unsigned char	*p;
	unsigned char	c_loop_invariant;

	p = (unsigned char *)s;
	c_loop_invariant = (unsigned char)c;
	while (n > 0)
	{
		*p = c_loop_invariant;
		p++;
		n--;
	}
	return (s);
}

/*
 * SYNOPSIS
 *       #include <string.h>
 *
 *       void *memset(void *s, int c, size_t n);
 *
 * DESCRIPTION
 *       The memset() function fills the first n bytes of the
 *       memory area pointed to by s with the constant byte c.
 *
 * RETURN VALUE
 *       The memset() function returns a pointer to the memory area s.
 */
