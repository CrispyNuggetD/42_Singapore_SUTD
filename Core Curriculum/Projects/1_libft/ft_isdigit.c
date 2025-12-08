/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isdigit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 12:06:28 by hnah              #+#    #+#             */
/*   Updated: 2025/12/08 12:09:07 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}

/*
int main(void)
{
	write(1, ft_itoa(ft_isdigit('a')), 1); 
	write(1, "\n", 1); 
	write(1, ft_itoa(ft_isdigit('1')), 1);
	write(1, "\n", 1);
	write(1, ft_itoa(ft_isdigit('/')), 1);
	return (0);
}
main tested
 */
