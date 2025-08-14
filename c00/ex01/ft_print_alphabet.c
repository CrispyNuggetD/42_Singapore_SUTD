/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_alphabet.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 10:47:14 by hnah              #+#    #+#             */
/*   Updated: 2025/07/24 11:46:39 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h> 

void	ft_print_alphabet(void)
{
	char	c;
	int		start;

	start = 97;
	while (start >= 97 && start <= 122)
	{
		c = (char)start;
		write(1, &c, 1);
		start++;
	}
}
