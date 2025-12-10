/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 13:34:47 by hnah              #+#    #+#             */
/*   Updated: 2025/12/10 13:44:03 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	return (dest);
}

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
 *        The  memmove() function copies n bytes from memory area src to memory area dest.
 *        The memory areas may overlap: copying takes place as though the bytes in src are first copied into a temporary array
 *        that does not overlap src or dest, and the bytes are then copied from the temporary array to dest.
 * 
 * RETURN VALUE
 * 		The memmove() function returns a pointer to dest.
 */
