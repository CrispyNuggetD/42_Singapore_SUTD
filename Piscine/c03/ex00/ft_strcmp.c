/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:20:05 by hnah              #+#    #+#             */
/*   Updated: 2025/08/07 10:11:42 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <stdio.h>
//#include <string.h>
//#include <unistd.h>

int	ft_strcmp(char *s1, char *s2)
{
	while (*s1 || *s2)
	{
		if ((unsigned char)*s1 != (unsigned char)*s2)
			return ((unsigned char)*s1 - (unsigned char)*s2);
		s1++;
		s2++;
	}
	return (0);
}

/*int	main(void)
{
	int	val;
	int	val2;
	char	*s1 = "abc";

	val = ft_strcmp(s1, s2);
	printf("help lah: %i", val);
	val2 = strcmp(s1, s2);
	printf("string.h: %i", val2);
}*/
