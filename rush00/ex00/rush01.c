/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rush01.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andng <andng@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 13:41:09 by andng             #+#    #+#             */
/*   Updated: 2025/07/27 18:13:44 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

void	ft_putchar(char c);

void	first_or_last_line_dots(int is_first_line, int x)
{
	int	cur_x;

	if (is_first_line == 1)
		ft_putchar(47);
	else
		ft_putchar(92);
	cur_x = 2;
	while (cur_x < x)
	{
		ft_putchar(42);
		cur_x++;
	}
	if (x == 1)
		;
	else if (is_first_line == 1)
		ft_putchar(92);
	else
		ft_putchar(47);
	write(1, "\n", 1);
}

void	other_lines(int x)
{
	int	cur_x;

	ft_putchar(42);
	cur_x = 2;
	while (cur_x < x)
	{
		write(1, " ", 1);
		cur_x++;
	}
	if (x != 1)
		ft_putchar(42);
	write(1, "\n", 1);
}

void	rush(int x, int y)
{
	int		cur_y;

	if (x == 1 && y == 1)
	{
		ft_putchar(47);
		write(1, "\n", 1);
	}
	else
	{
		cur_y = 1;
		while (cur_y <= y)
		{
			if (cur_y == 1)
				first_or_last_line_dots(1, x);
			else if (cur_y == y)
				first_or_last_line_dots(0, x);
			else
				other_lines(x);
			cur_y++;
		}
	}
}
