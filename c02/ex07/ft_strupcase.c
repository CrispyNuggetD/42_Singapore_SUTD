/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strupcase.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 16:15:56 by hnah              #+#    #+#             */
/*   Updated: 2025/08/04 18:48:07 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <stdio.h>
//#include <unistd.h>

char	*ft_strupcase(char *str)
{
	char	*start;

	start = str;
	while (*str)
	{
		if (*str >= 'a' && *str <= 'z')
			*str = *str - 32;
		str++;
	}
	return (start);
}

/*int	main(void)
{
	char	test[17] = "zz 11 q AA wdq\nw";
	char	*result = ft_strupcase(test);
	printf("result is: %s", result);
}*/

/* Output: ZZ 11 Q AA WDQ
W%
*/
