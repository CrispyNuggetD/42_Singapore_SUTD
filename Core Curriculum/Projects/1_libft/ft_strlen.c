/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 11:29:18 by hnah              #+#    #+#             */
/*   Updated: 2025/12/07 15:17:07 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlen(const char *s)
{
	size_t	len;

	len = 0;
	while (*s)
	{
		len++;
		s++;
	}
	return (len);
}

/*
int	main(void)
{
	char	*input_1;
	char	*input_2;
	char	*input_3;
	char	*input_4;

	input_1 = "";
	input_2 = "A";
	input_3 = "42";
	input_4 = "Hello World! 42 is Aw#s0m@";
	write(1, "\ninput_1 length is: \n", 21);
	write(1, ft_itoa(ft_strlen(input_1)),
		ft_atoi(ft_itoa(rt_strlen(input_1))) + 1);
	write(1, "\ninput_2 length is: \n", 21);
	write(1, ft_itoa(ft_strlen(input_2)),
		ft_atoi(ft_itoa(ft_strlen(input_2))) + 1);
	write(1, "\ninput_3 length is: \n", 21);
	write(1, ft_itoa(ft_strlen(input_3)),
		ft_atoi(ft_itoa(ft_strlen(input_3))) + 1);
	write(1, "\ninput_4 length is: \n", 21);
	write(1, ft_itoa(ft_strlen(input_4)),
		ft_atoi(ft_itoa(ft_strlen(input_4))) + 1);
}
*/

// if len = 27
// itoa becomes "27", can write
// atoi of the itoa gives "2" can know number of chars.
// +1 to include the null terminator 
// (in case len is 0, we don't write "0 bytes")
