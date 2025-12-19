/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalpha.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:40:07 by hnah              #+#    #+#             */
/*   Updated: 2025/12/11 13:50:17 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isalpha(int c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return (1);
	return (0);
}

/*
int	main(void)
{
	write(1, ft_itoa(ft_isalpha('a')), 1);
	write(1, "\n", 1);
	write(1, ft_itoa(ft_isalpha('1')), 1);
	write(1, "\n", 1);
	write(1, ft_itoa(ft_isalpha('/')), 1);
	return (0);
}
main tested
 */
