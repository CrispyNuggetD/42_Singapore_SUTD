#include "libft.h"

char		*ft_substr(char const *s, unsigned int start, size_t len)
{
	unsigned int	s_len;
	char const	*result;
	
	
	s_len = ft_strlen(s);
	if (len > s_len)
		return (ft_strdup(""));
	if (s_len > start + len)
		copy_len = len;
	else
		copy_len = s_len - start;
	result = strlcpy(dest???, s, copy_len + 1); 
}

// ft_strdup("") includes both malloc and null