/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   colour.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 05:57:05 by hnah              #+#    #+#             */
/*   Updated: 2026/08/26 05:57:05 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

static int	hex_value(char character)
{
	if (character >= '0' && character <= '9')
		return (character - '0');
	if (character >= 'a' && character <= 'f')
		return (character - 'a' + 10);
	if (character >= 'A' && character <= 'F')
		return (character - 'A' + 10);
	return (-1);
}

int	parse_colour(char *word)
{
	int	colour;
	int	index;
	int	digit;

	index = 0;
	while (word[index] != '\0' && word[index] != ',')
		index++;
	if (word[index] == '\0')
		return (WHITE);
	index++;
	if (word[index] == '0' && (word[index + 1] == 'x'
			|| word[index + 1] == 'X'))
		index += 2;
	colour = 0;
	digit = hex_value(word[index]);
	while (digit >= 0)
	{
		colour = colour * 16 + digit;
		index++;
		digit = hex_value(word[index]);
	}
	return (colour);
}

static int	blend_channel(int start, int end, int step, int steps)
{
	int	start_value;
	int	end_value;

	start_value = start & 0xFF;
	end_value = end & 0xFF;
	return (start_value + (end_value - start_value) * step / steps);
}

int	blend_colour(t_point start, t_point end, int step, int steps)
{
	int	red;
	int	green;
	int	blue;

	red = blend_channel(start.colour >> 16, end.colour >> 16, step, steps);
	green = blend_channel(start.colour >> 8, end.colour >> 8, step, steps);
	blue = blend_channel(start.colour, end.colour, step, steps);
	return ((red << 16) | (green << 8) | blue);
}
