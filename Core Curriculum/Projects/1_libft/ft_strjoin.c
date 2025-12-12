/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 15:54:05 by hnah              #+#    #+#             */
/*   Updated: 2025/12/12 16:52:49 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char    *result;
	size_t	s1_len;
	size_t	s2_len;

	if (!s1 || !s2)
		return (NULL);
	s1_len = ft_strlen(s1);
	s2_len = ft_strlen(s2);
	if (s1_len > SIZE_MAX - s2_len - 1)
    	return (NULL);
	result = malloc(sizeof(char) * (s1_len + s2_len + 1));
	if (!result)
		return (NULL);
	ft_strlcpy(result, s1, (s1_len + s2_len + 1));
	ft_strlcat(result, s2, (s1_len + s2_len + 1));
	return (result);
}

/*
 * if (s1_len > SIZE_MAX - s2_len - 1) guards size_t overflow.
 * (s1_len + s2_len + 1) Bracketed for clarity.
 *
 * First strlcpy copies s1 to "available entire buffer size".
 * copy first, then concatenate.
 *
 * Design Process:
 * TL;DR - 	I stayed with strlcpy/strlcat after all, not memcpy. 
 * 			It's the syscall that's slow, not so much strl*.
 * 
 * Ok i originally did:
 * ft_strlcpy(result, s1, (s1_len + 1));
 * ft_strlcat(result, s2, (s1_len + s2_len + 1));
 * Which is ok (I just need to rmb +1 for null terminator).
 *
 * BUT it's apparently inefficient cos it checks length inside AGAIN?
 *
 * So yea, RAW memcpy here would be:
 * ft_memcpy(result, s1, s1_len);
 * ft_memcpy(result + s1_len, s2, s2_len);
 * result[s1_len + s2_len] = '\0';
 * (remember that's the null terminator location based on indexing).
 *
 * BUT I anyway realised (From ChatGPT):
 * - memcpy mistakes often are SILENT corruption (worst kind).
 * - strl* tends to produce truncation when size is too small, 
 * 		which can be easier to notice in output (still a bug, but often
 * 		less catastrophic than a heap smash).
 *
 * So: strl* fails “softer” in many misuse cases.
 *
 * SO anyway I went back to strlcpy/strlcat.
 */
