/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 16:49:40 by hnah              #+#    #+#             */
/*   Updated: 2025/12/12 18:07:51 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*result;
	size_t	length;
	size_t	end;
	size_t	i;
	
	if (!s1 || !set)
		return (NULL);
	i = 0;
	while (s1[i] && ft_strchr(set, s1[i]))
		i++;
	end = ft_strlen(s1);
	while (end > i && ft_strchr(set, s1[end - 1]))
    	end--;
	length = end - i;
	result = ft_substr(s1, i, length);
	return (result);	
}

/*
 * ft_strchr(set, s1[i]) basically loops index of main str s1 until NOT
 * in set; NULL returned and break, then we know start/ end.
 *
 * end > i, checks they don't intersect (empty str if does)
 */
