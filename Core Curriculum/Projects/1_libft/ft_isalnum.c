/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalnum.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 12:13:58 by hnah              #+#    #+#             */
/*   Updated: 2025/12/11 13:50:31 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isalnum(int c)
{
	if ((c >= '0' && c <= '9')
		|| (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return (1);
	return (0);
}

/*
int	main(void)
{
	write(1, ft_itoa(ft_isalnum('a')), 1);
	write(1, "\n", 1);
	write(1, ft_itoa(ft_isalnum('1')), 1);
	write(1, "\n", 1);
	write(1, ft_itoa(ft_isalnum('/')), 1);
	return (0);
}
main tested
 */
