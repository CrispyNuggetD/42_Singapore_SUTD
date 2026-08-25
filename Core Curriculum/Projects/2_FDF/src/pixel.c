/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pixel.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 23:33:59 by hnah              #+#    #+#             */
/*   Updated: 2026/08/25 23:33:59 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

void	fdf_put_pixel(t_image *image, int x, int y, unsigned int colour)
{
	char	*pixel;
	int		bytes_per_pixel;
	int		index;

	if (x < 0 || x >= WIN_WIDTH || y < 0 || y >= WIN_HEIGHT)
		return ;
	bytes_per_pixel = image->bits_per_pixel / 8;
	pixel = image->addr + y * image->line_length + x * bytes_per_pixel;
	index = 0;
	while (index < bytes_per_pixel)
	{
		if (image->endian == 0)
			pixel[index] = (colour >> (index * 8)) & 0xFF;
		else
			pixel[bytes_per_pixel - 1 - index]
				= (colour >> (index * 8)) & 0xFF;
		index++;
	}
}
