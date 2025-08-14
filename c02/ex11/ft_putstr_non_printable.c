/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr_non_printable.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 16:54:38 by hnah              #+#    #+#             */
/*   Updated: 2025/08/10 16:24:55 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
//#include <stdio.h>

void	ft_putstr_non_printable(char *str)
{
	char			*base16;
	char			div;
	char			modulo;
	unsigned char	c;

	base16 = "0123456789abcdef";
	while (*str)
	{
		c = (unsigned char)*str;
		if (c < 32 || c >= 127)
		{
			div = base16[c / 16];
			modulo = base16[c % 16];
			write(1, "\\", 1);
			write(1, &div, 1);
			write(1, &modulo, 1);
		}
		else
			write(1, str, 1);
		str++;
	}
}

/*int	main(void)
{
	char	str[] = "Hello\x7fHow are you? ";
	
	ft_putstr_non_printable(str);
}*/
