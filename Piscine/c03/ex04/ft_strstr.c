/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strstr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 10:34:08 by hnah              #+#    #+#             */
/*   Updated: 2025/08/11 13:09:49 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <unistd.h>
//#include <stdio.h>

char	*ft_strstr(char *str, char *to_find)
{
	char	*f;
	char	*s;

	if (*to_find == '\0')
		return (str);
	while (*str)
	{
		s = str;
		f = to_find;
		while (*s && *f && (*s == *f))
		{
			s++;
			f++;
		}
		if (*f == '\0')
			return (str);
		str++;
	}
	return (0);
}

/*int	main(void)
{
	printf("%s\n", ft_strstr("Piscine42Singapore", "42"));
	// Output: 42Singapore

	printf("%s\n", ft_strstr("Piscine42Singapore", "Singapore"));
	// Output: Singapore

	printf("%s\n", ft_strstr("Piscine42Singapore", ""));
	// Output: Piscine42Singapore

	printf("%s\n", ft_strstr("Piscine42Singapore", "hello"));
	// Output: (null)
}*/
