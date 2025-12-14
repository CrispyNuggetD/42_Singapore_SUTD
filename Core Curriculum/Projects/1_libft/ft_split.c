/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 13:37:36 by hnah              #+#    #+#             */
/*   Updated: 2025/12/13 18:18:40 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/* size_t	ft_strlcpy(char *dst, const char *src, size_t size)
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
} */

char	**ft_split(char const *s, char c)
{
	char	**result;
	size_t	words;
	size_t	len;
	size_t	end;
	
	words = 0;
	len = 0;
	if (!s)
		return (NULL);
	/* if (&s[len] == NULL)
	{
		result = malloc(sizeof(char *) * (words + 1));
		if (!result)
			return (NULL);
		result[words] = NULL;
		return (result);
	} */
	while (s[len] == c && s[len])
		len++;
	while (s[len])
	{
		while (s[len] && s[len] != c)
			len++;
		words++;
		while (s[len] && s[len] == c)
        	len++;
	}
	result = malloc(sizeof(char *) * (words + 1));
	if (!result)
		return (NULL);
	result[words] = NULL;
	while (s[len] == c || s[len] == '\0')
		len--;
	while (words > 0)
	{
		end = len + 1;
		while (s[len - 1] != c && (len > 0))
			len--;
		result[words - 1] = malloc(sizeof(char) * (end - len + 1));
		if (!result[words - 1])
		{
			while (words != 0)
				free(result[words--]);
			free(result);
			return (NULL);
		}
		ft_strlcpy(result[words - 1], &s[len], end - len + 1);
			len--;
		while (s[len] == c && (len > 0))
			len--;
		words--;
	}
	return (result);
}

/* int	main(void)
{
	char	**test;
	size_t	i;

	i = 0;
	test = ft_split("", ' ');
	while (test[i])
	{
		printf("%zu | %s,\n\n", i, test[i]);
		i++;
	}
	while (i > 0)
	{
		free(test[i--]);
	}
	free(test[0]);
	free(test);
} */

// 0		1		2:c		3	4	5		\0
// 0		1		2:start	3	4	end:5	\0
// 
// Str. length = (5-2) = 3
// Needed for null terminator = 1
// Hence, total to malloc/ strlcpy = (5-2) + 1