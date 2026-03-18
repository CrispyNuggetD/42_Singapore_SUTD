/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 13:37:36 by hnah              #+#    #+#             */
/*   Updated: 2025/12/17 16:37:12 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
//#include <stdio.h>

static char	**factory(char const *s, char c, size_t words, size_t len);
static void	*all_freer(char **result, size_t words);

char	**ft_split(char const *s, char c)
{
	size_t	words;
	size_t	len;

	words = 0;
	len = 0;
	if (!s)
		return (NULL);
	while (s[len] == c && s[len])
		len++;
	while (s[len])
	{
		words++;
		while (s[len] && s[len] != c)
			len++;
		while (s[len] && s[len] == c)
			len++;
	}
	return (factory(s, c, words, len));
}

static char	**factory(char const *s, char c, size_t words, size_t len)
{
	size_t	end;
	char	**result;

	result = malloc(sizeof(char *) * (words + 1));
	if (!result)
		return (NULL);
	result[words] = NULL;
	while (len > 0 && (s[len] == c || s[len] == '\0'))
		len--;
	while (words > 0)
	{
		end = len + 1;
		while ((len > 0) && s[len - 1] != c)
			len--;
		result[words - 1] = malloc(sizeof(char) * (end - len + 1));
		if (!result[words - 1])
			return (all_freer(result, words));
		ft_strlcpy(result[words - 1], &s[len], end - len + 1);
		if (len > 0)
			len--;
		while ((len > 0) && s[len] == c)
			len--;
		words--;
	}
	return (result);
}

static void	*all_freer(char **result, size_t words)
{
	while (result[words] != NULL)
		free(result[words++]);
	free(result);
	return (NULL);
}

/* int	main(void)
{
	char	**test;
	size_t	i;

	i = 0;
	test = ft_split(" ewar asd qw gfgg qqq d     ''f ^^&    ", ' ');
	while (test[i])
	{
		printf("%zu | %s \n\n", i, test[i]);
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
