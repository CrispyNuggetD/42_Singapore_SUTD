/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlowcase.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 16:15:56 by hnah              #+#    #+#             */
/*   Updated: 2025/08/04 19:08:20 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <stdio.h>
//#include <unistd.h>

char	*ft_strlowcase(char *str)
{
	char	*start;

	start = str;
	while (*str)
	{
		if (*str >= 'A' && *str <= 'Z')
			*str = *str + 32;
		str++;
	}
	return (start);
}

/*int	main(void)
{
	char	test[17] = "zz 11 q AA wdq\nw";
	char	*result = ft_strlowcase(test);
	printf("result is: %s", result);
}*/

/* Output:
 * result is: zz 11 q aa wdq
w%
*/
