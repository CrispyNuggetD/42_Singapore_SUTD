/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 15:50:20 by hnah              #+#    #+#             */
/*   Updated: 2025/12/12 16:34:38 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*result;
	size_t	copy_len;
	size_t	s_len;

	if (!s)
		return (NULL);
	s_len = ft_strlen(s);
	if (start >= s_len)
		return (ft_strdup(""));
	if (len > s_len - start)
		copy_len = s_len - start;
	else
		copy_len = len;
	result = malloc(sizeof(char) * (copy_len + 1));
	if (!result)
		return (NULL);
	ft_strlcpy(result, s + start, copy_len + 1);
	return (result);
}

// ft_strdup("") includes both malloc and null
// start is >= instead of > as start is using indexing from [0] of s
