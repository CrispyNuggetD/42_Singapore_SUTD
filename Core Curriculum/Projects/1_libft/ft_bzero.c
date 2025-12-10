/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 18:06:55 by hnah              #+#    #+#             */
/*   Updated: 2025/12/10 12:56:01 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_bzero(void *s, size_t n)
{
	unsigned char	*p;

	p = (unsigned char *)s;
	while (n > 0)
	{
		*p = '\0';	
		p++;
		n--;
	}
}

/*
 * NAME
 *        bzero, explicit_bzero - zero a byte string
 * 
 * SYNOPSIS
 *        #include <strings.h>
 * 
 *        void bzero(void *s, size_t n);
 * 
 *        #include <string.h>
 * 
 *        void explicit_bzero(void *s, size_t n);
 * 
 * DESCRIPTION
 *        The bzero() function erases the data in the n bytes of the memory starting at the location pointed to by s, by writing zeros (bytes containing '\0') to that area.
 * 
 *        The  explicit_bzero()  function  performs the same task as bzero().  It differs from bzero() in that it guarantees that compiler optimizations will not remove the erase operation if the compiler de‐
 *        duces that the operation is "unnecessary".
 * 
 * RETURN VALUE
 *        None.
 */
