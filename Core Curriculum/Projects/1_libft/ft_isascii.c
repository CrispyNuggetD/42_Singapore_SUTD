/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isascii.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 12:25:35 by hnah              #+#    #+#             */
/*   Updated: 2025/12/08 12:25:43 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isascii(int c)
{
	if (c >= 0 && c <= 127)
		return (1);
	return (0);
}

/*
int main(void)
{
	write(1, ft_itoa(ft_isascii(0)), 1);
	write(1, "\n", 1);
	write(1, ft_itoa(ft_isascii(10)), 1);
	write(1, "\n", 1);
	write(1, ft_itoa(ft_isascii('\n')), 1);
	write(1, "\n", 1); 
	write(1, ft_itoa(ft_isascii(-1)), 1);
	write(1, "\n", 1);
	write(1, ft_itoa(ft_isascii(128)), 1);
	return (0);
}
main tested
 */
