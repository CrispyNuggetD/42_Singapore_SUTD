/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcat.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 18:29:21 by hnah              #+#    #+#             */
/*   Updated: 2025/08/07 11:42:48 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <string.h>
//#include <stdio.h>

char	*ft_strcat(char *dest, char *src)
{
	char	*start;

	start = dest;
	while (*dest)
		dest++;
	while (*src)
	{
		*dest = *src;
		dest++;
		src++;
	}
	*dest = '\0';
	return (start);
}

/*int	main(void)
{
	char	*result1;
	char	*result2;
	char	input1[20] = "test";
	char	input2[20] = "another";
	char	input1_2[20] = "test";

	result1 = strcat(input1, input2);
	printf("string.h is: %s\n", input1);
	result2 = ft_strcat(input1_2, input2);
	printf("mine dest is: %s\n", input1_2);
        printf("stored dest is: %s\n", result2);
}*/
