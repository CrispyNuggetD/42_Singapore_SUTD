/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 13:18:55 by hnah              #+#    #+#             */
/*   Updated: 2025/12/16 18:44:28 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	const unsigned char	*src_backup;
	unsigned char		*dest_backup;

	if (!dest && !src)
		return (NULL);
	src_backup = (unsigned char *)src;
	dest_backup = (unsigned char *)dest;
	while (n--)
	{
		*dest_backup = *src_backup;
		dest_backup++;
		src_backup++;
	}
	return (dest);
}

/*
 * NAME
 *        memcpy - copy memory area
 * 
 * SYNOPSIS
 *        #include <string.h>
 * 
 *        void *memcpy(void *dest, const void *src, size_t n);
 * 
 * DESCRIPTION
 *        The memcpy() function copies n bytes from memory area src
 *        to memory area dest. The memory areas must not overlap.
 *        Use memmove(3) if the memory areas do overlap.
 * 
 * RETURN VALUE
 *        The memcpy() function returns a pointer to dest.
 */
