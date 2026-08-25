/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   projection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 02:51:15 by hnah              #+#    #+#             */
/*   Updated: 2026/08/26 05:50:57 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "projection.h"

t_projected	project_model(t_point point, t_map *map,
		t_projection *projection)
{
	t_projected	projected;
	double		model_x;
	double		model_y;

	model_x = point.x - (map->width - 1) / 2.0;
	model_y = point.y - (map->height - 1) / 2.0;
	projected.x = (model_x - model_y) * projection->cos_angle;
	projected.y = (model_x + model_y) * projection->sin_angle - point.z;
	return (projected);
}

static t_point	project_point(t_point point, t_map *map,
		t_projection *projection)
{
	t_projected	projected;

	projected = project_model(point, map, projection);
	point.x = round(projected.x * projection->scale + projection->offset_x);
	point.y = round(projected.y * projection->scale + projection->offset_y);
	return (point);
}

static void	draw_neighbours(t_info *info, int index,
		t_projection *projection)
{
	t_point	current;
	t_point	neighbour;

	current = project_point(info->map.points[index], &info->map, projection);
	if (info->map.points[index].x + 1 < info->map.width)
	{
		neighbour = project_point(info->map.points[index + 1], &info->map,
				projection);
		draw_line(&info->image, current, neighbour);
	}
	if (info->map.points[index].y + 1 < info->map.height)
	{
		neighbour = project_point(info->map.points[index + info->map.width],
				&info->map, projection);
		draw_line(&info->image, current, neighbour);
	}
}

void	render_map(t_info *info)
{
	t_projection	projection;
	int				index;

	projection = init_projection(&info->map);
	index = 0;
	while (index < info->map.width * info->map.height)
	{
		draw_neighbours(info, index, &projection);
		index++;
	}
}
