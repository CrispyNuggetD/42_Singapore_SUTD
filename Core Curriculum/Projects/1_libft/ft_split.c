#include "libft.h"

char	**ft_split(char const *s, char c)
{
	char	**result;
	char	*temp;
	size_t	words
	size_t	len;
	size_t	end;
	
	words = 0;
	len = 0;
	if (!s)
		return (NULL);
	while (s[len])
	{
		if (s[len] == c && len != 0 && s[len] != s [len - 1])
			words += 1;
		len++;
	}
	result = malloc(sizeof(char *) * (words + 1));
	if (!result)
		return (NULL);
	result[words] = NULL;
	while (words > 0)
	{
		while (s[len] == c && s[len] == '\0')
			len--;
		end = len;
		while (s[len] != c && len > 0）
			len--;
		temp = malloc(sizeof(char) * (end - len + 2));
		if (!temp)
		{
			while (words >= 0)
				free(result[words--]);
			free(result);
			return (NULL);
		}
		strlcpy(result[words - 1], s[len], end - len + 2);
		words--;
	}
}

// 0:A		1:B		2:c		3	4	5
// len:0	end:1	3:c		3	4	5
// 
// Str. length = (1-0) + 1
// Needed for null terminator = 1
// Buffer w. null = 1 + 1 = 2
// Hence, total to malloc/ strlcpy = (1-0) + 2
//
// Started this w/o. Stdheader (to update) at 1:37pm 13 Dec Sat 2025