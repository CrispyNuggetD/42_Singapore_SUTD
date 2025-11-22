/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcapitalize.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 16:15:56 by hnah              #+#    #+#             */
/*   Updated: 2025/08/10 16:36:20 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <stdio.h>
//#include <unistd.h>

char	*make_str_lowercase(char *str)
{
	char	*ptr;

	ptr = str;
	while (*str)
	{
		if (*str >= 'A' && *str <= 'Z')
			*str = *str + 32;
		str++;
	}
	return (ptr);
}

char	*ft_strcapitalize(char *str)
{
	char	*start;
	char	prev;

	prev = '\0';
	str = make_str_lowercase(str);
	start = str;
	while (*str)
	{
		if ((!(prev >= '0' && prev <= '9'))
			&& (!(prev >= 'a' && prev <= 'z'))
			&& (!(prev >= 'A' && prev <= 'Z'))
			&& (*str >= 'a' && *str <= 'z'))
			*str = *str - 32;
		prev = *str;
		str++;
	}
	return (start);
}

/*int	main(void)
{
	char	test[] = "hi, HOW are you?
	//	42words forty-two; fifty+and+one  ....Aa ...bb";
	char	*result = ft_strcapitalize(test);
	printf("result is: %s", result);
}*/

/* Output:
 * result is: zz 11 q aa wdq
w%
*/
