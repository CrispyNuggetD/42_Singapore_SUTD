/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   line.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 04:15:32 by hnah              #+#    #+#             */
/*   Updated: 2026/08/26 04:15:41 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

static int	absolute(int value)
{
	if (value < 0)
		return (-value);
	return (value);
}

static void	draw_x_dominant(t_image *image, t_point start, t_point end)
{
	double	y;
	double	y_step;
	int		direction;

	direction = 1;
	if (end.x < start.x)
		direction = -1;
	y = start.y;
	y_step = (double)(end.y - start.y) / absolute(end.x - start.x);
	while (start.x != end.x)
	{
		fdf_put_pixel(image, start.x, (int)round(y), 0x00FFFFFF);
		start.x += direction;
		y += y_step;
	}
	fdf_put_pixel(image, end.x, end.y, 0x00FFFFFF);
}

static void	draw_y_dominant(t_image *image, t_point start, t_point end)
{
	double	x;
	double	x_step;
	int		direction;

	direction = 1;
	if (end.y < start.y)
		direction = -1;
	x = start.x;
	x_step = (double)(end.x - start.x) / absolute(end.y - start.y);
	while (start.y != end.y)
	{
		fdf_put_pixel(image, (int)round(x), start.y, 0x00FFFFFF);
		start.y += direction;
		x += x_step;
	}
	fdf_put_pixel(image, end.x, end.y, 0x00FFFFFF);
}

void	draw_line(t_image *image, t_point start, t_point end)
{
	int	dx;
	int	dy;

	dx = absolute(end.x - start.x);
	dy = absolute(end.y - start.y);
	if (dx == 0 && dy == 0)
		fdf_put_pixel(image, start.x, start.y, 0x00FFFFFF);
	else if (dx >= dy)
		draw_x_dominant(image, start, end);
	else
		draw_y_dominant(image, start, end);
}
