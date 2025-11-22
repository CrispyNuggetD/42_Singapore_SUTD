/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 20:05:38 by hnah              #+#    #+#             */
/*   Updated: 2025/08/10 15:13:43 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <stdio.h>
//#include <unistd.h>

unsigned int	ft_strlcpy(char *dest, char *src, unsigned int size)
{
	unsigned int	src_length;
	unsigned int	i;

	src_length = 0;
	i = 0;
	while (src[src_length])
		src_length++;
	if (size == 0)
		return (src_length);
	while (src[i] && (i < size - 1))
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (src_length);
}

/*int	main(void)
{
	char	src[] = "test";
	char	dest[] = "asbhwaeef";
	unsigned int	src_length;
	
	src_length = ft_strlcpy(dest, src, 3);
	printf("dest is: %s ", dest);
	printf("source length is: %i", src_length);
}*/
