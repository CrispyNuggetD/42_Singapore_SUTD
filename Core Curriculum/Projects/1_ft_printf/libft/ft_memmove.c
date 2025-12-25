/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 13:34:47 by hnah              #+#    #+#             */
/*   Updated: 2025/12/11 12:57:22 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	const unsigned char	*src_backup;
	unsigned char		*dest_backup;

	src_backup = (const unsigned char *)src;
	dest_backup = (unsigned char *)dest;
	if (!dest && !src)
		return (NULL);
	if (dest == src || n == 0)
		return (dest);
	if (dest_backup < src_backup)
	{
		while (n--)
			*dest_backup++ = *src_backup++;
	}
	else
	{
		while (n--)
			dest_backup[n] = src_backup[n];
	}
	return (dest);
}

// Additional Notes:
// Technically comparing null ptr in C is not allowed
// (But irl nothing crashes really)
// but anyway, (!dest && !src) is guard for input (NULL, NULL, 0).
//
// Also, technically comparing NULL and another ptr is UB
// by C standard but on POSIX/ 42 comp normally just lowest address.
// 
// Cos the C standard says:
// If dest == NULL OR src == NULL AND n > 0 the behaviour is undefined. 
// 
// So this is legal:
// memmove(NULL, NULL, 0); 
// Since !(n > 0:
// 
// Hence, for correctness, we put (dest == src || n == 0)

/*
 * NAME
 *        memmove - copy memory area
 * 
 * SYNOPSIS
 *        #include <string.h>
 * 
 *        void *memmove(void *dest, const void *src, size_t n);
 * 
 * DESCRIPTION
 *        The  memmove() function copies n bytes from
 *        memory area src to memory area dest.
 *        The memory areas may overlap: copying takes place as though
 *        the bytes in src are first copied into a temporary array
 *        that does not overlap src or dest, and the bytes are then copied
 *        from the temporary array to dest.
 * 
 * RETURN VALUE
 * 		The memmove() function returns a pointer to dest.
 */
