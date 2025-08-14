/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:58:02 by hnah              #+#    #+#             */
/*   Updated: 2025/08/11 09:58:04 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <stdio.h>
//#include <string.h>

int	ft_strncmp(char *s1, char *s2, unsigned int n)
{
	unsigned int	i;

	i = 0;
	while ((s1[i] || s2[i]) && i < n)
	{
		if ((unsigned char)s1[i] != (unsigned char)s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	return (0);
}

/*int	main(void)
{
	int	val1;
	int	val2;
	char	*s1 = "te";
	char	*s2 = "tez";

	val1 = strncmp(s1, s2, 3);
	val2 = ft_strncmp(s1, s2, 3);
	printf("string.h: %i", val1);
	printf("mine: %i", val2);
}*/
