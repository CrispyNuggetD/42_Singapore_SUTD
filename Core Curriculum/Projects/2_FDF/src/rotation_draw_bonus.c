/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rotation_draw_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 00:00:00 by hnah              #+#    #+#             */
/*   Updated: 2026/09/04 00:00:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rotation_bonus.h"

static void	draw_rotation_neighbours(t_rotation *rotation, int index,
		t_projection *projection)
{
	t_point	current;
	t_point	neighbour;

	current = rotation_point(rotation->info.map.points[index], rotation,
			projection);
	if (rotation->info.map.points[index].x + 1 < rotation->info.map.width)
	{
		neighbour = rotation_point(rotation->info.map.points[index + 1],
				rotation, projection);
		draw_line(&rotation->info.image, current, neighbour);
	}
	if (rotation->info.map.points[index].y + 1 < rotation->info.map.height)
	{
		neighbour = rotation_point(rotation->info.map.points[index
				+ rotation->info.map.width], rotation, projection);
		draw_line(&rotation->info.image, current, neighbour);
	}
}

void	rotation_render(t_rotation *rotation)
{
	t_projection	projection;
	int				index;

	index = 0;
	while (index < rotation->info.image.line_length * WIN_HEIGHT)
		rotation->info.image.addr[index++] = 0;
	projection = rotation_projection(rotation);
	index = 0;
	while (index < rotation->info.map.width * rotation->info.map.height)
	{
		draw_rotation_neighbours(rotation, index, &projection);
		index++;
	}
	mlx_put_image_to_window(rotation->info.mlx, rotation->info.win,
		rotation->info.image.ptr, 0, 0);
}
