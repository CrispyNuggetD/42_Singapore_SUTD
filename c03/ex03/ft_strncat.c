/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 09:49:41 by hnah              #+#    #+#             */
/*   Updated: 2025/08/07 11:50:36 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <unistd.h>
//#include <stdio.h>

char	*ft_strncat(char *dest, char *src, unsigned int nb)
{
	char			*start;
	unsigned int	i;

	start = dest;
	i = 0;
	while (*dest)
		dest++;
	while (src[i] && i < nb)
	{
		*dest = src[i];
		i++;
		dest++;
	}
	*dest = '\0';
	return (start);
}

/*int	main(void)
{
	char	dest[10] = "ab";
	char	src[5] = "test";
	char	*s;

	s = ft_strncat(dest, src, 3);
	printf("dest is: %s\n", s);
}*/
