/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 12:01:24 by hnah              #+#    #+#             */
/*   Updated: 2025/12/11 12:57:46 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t	copied;
	size_t	len_src;

	len_src = 0;
	copied = 0;
	while (src[len_src])
		len_src++;
	if (size == 0)
		return (len_src);
	while (src[copied] && (copied + 1 < size))
	{
		dst[copied] = src[copied];
		copied++;
	}
	dst[copied] = '\0';
	return (len_src);
}
