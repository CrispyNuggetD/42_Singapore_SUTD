/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 11:51:56 by hnah              #+#    #+#             */
/*   Updated: 2025/08/10 20:00:56 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <unistd.h>
//#include <stdio.h>

/* This exercise took way longer than it should have, 
 * AND I still don't know if it will pass moulinette
 * sadly... grrrrrr... anyway:
 * 
 * Personal notes:
 * If the return value is >= size, truncation occurred.
 * It is the length of the string it tried to create.
 *
 * There are 2 cases this can happen:
 * Case 1: size > dest length → returns dest len + srclen (normal append
 * Case 2: size <= dest length → returns 
 * size + src_length (no append, report truncation).*/

unsigned int	ft_strlcat(char *dest, char *src, unsigned int size)
{
	unsigned int	i;
	unsigned int	a;
	unsigned int	b;

	i = 0;
	a = 0;
	b = 0;
	while (dest[a])
		a++;
	while (src[b])
		b++;
	if (a >= size)
		return (size + b);
	while (src[i] && size > a + i + 1)
	{
		dest[a + i] = src[i];
		i++;
	}
	dest[a + i] = '\0';
	return (a + b);
}

/*int main(void)
{
	char *src = "test";
	char dest[10] = "ab";
	int length = 0;
	
	length = ft_strlcat(dest, src,20);
	printf("output: %s", dest);
	printf("length: %i", length);
}*/
