/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rush03.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andng <andng@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 13:41:09 by andng             #+#    #+#             */
/*   Updated: 2025/07/27 18:19:43 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

void	ft_putchar(char c);

void	first_or_last_line_dots(int x)
{
	int	cur_x;

	ft_putchar(65);
	cur_x = 2;
	while (cur_x < x)
	{
		ft_putchar(66);
		cur_x++;
	}
	if (x == 1)
		;
	else
		ft_putchar(67);
	write(1, "\n", 1);
}

void	other_lines(int x)
{
	int	cur_x;

	ft_putchar(66);
	cur_x = 2;
	while (cur_x < x)
	{
		write(1, " ", 1);
		cur_x++;
	}
	if (x != 1)
		ft_putchar(66);
	write(1, "\n", 1);
}

void	rush(int x, int y)
{
	int		cur_y;

	if (x == 1 && y == 1)
	{
		ft_putchar(65);
		write(1, "\n", 1);
	}
	else
	{
		cur_y = 1;
		while (cur_y <= y)
		{
			if (cur_y == 1)
				first_or_last_line_dots(x);
			else if (cur_y == y)
				first_or_last_line_dots(x);
			else
				other_lines(x);
			cur_y++;
		}
	}
}
