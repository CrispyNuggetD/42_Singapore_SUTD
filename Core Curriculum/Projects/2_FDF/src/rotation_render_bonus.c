/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rotation_render_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 00:00:00 by hnah              #+#    #+#             */
/*   Updated: 2026/09/04 00:00:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rotation_bonus.h"

static t_projected	rotate_project(t_point point, t_map *map, double angle)
{
	t_projected	projected;
	double		x;
	double		y;
	double		rotated_x;
	double		rotated_y;

	x = point.x - (map->width - 1) / 2.0;
	y = point.y - (map->height - 1) / 2.0;
	rotated_x = x * cos(angle) - y * sin(angle);
	rotated_y = x * sin(angle) + y * cos(angle);
	projected.x = (rotated_x - rotated_y) * cos(PI / 6.0);
	projected.y = (rotated_x + rotated_y) * sin(PI / 6.0) - point.z;
	return (projected);
}

static void	update_rotation_bounds(t_bounds *bounds, t_projected point)
{
	if (point.x < bounds->min_x)
		bounds->min_x = point.x;
	if (point.x > bounds->max_x)
		bounds->max_x = point.x;
	if (point.y < bounds->min_y)
		bounds->min_y = point.y;
	if (point.y > bounds->max_y)
		bounds->max_y = point.y;
}

static t_bounds	rotation_bounds(t_rotation *rotation)
{
	t_bounds		bounds;
	t_projected		point;
	int				index;

	point = rotate_project(rotation->info.map.points[0],
			&rotation->info.map, rotation->angle);
	bounds.min_x = point.x;
	bounds.max_x = point.x;
	bounds.min_y = point.y;
	bounds.max_y = point.y;
	index = 1;
	while (index < rotation->info.map.width * rotation->info.map.height)
	{
		point = rotate_project(rotation->info.map.points[index],
				&rotation->info.map, rotation->angle);
		update_rotation_bounds(&bounds, point);
		index++;
	}
	return (bounds);
}

t_projection	rotation_projection(t_rotation *rotation)
{
	t_projection	projection;
	t_bounds		bounds;
	double			width;
	double			height;

	bounds = rotation_bounds(rotation);
	width = bounds.max_x - bounds.min_x;
	height = bounds.max_y - bounds.min_y;
	projection.scale = WIN_WIDTH - WINDOW_MARGIN;
	if (width > 0.0)
		projection.scale /= width;
	if (height > 0.0 && (width == 0.0 || (WIN_HEIGHT - WINDOW_MARGIN)
			/ height < projection.scale))
		projection.scale = (WIN_HEIGHT - WINDOW_MARGIN) / height;
	projection.offset_x = WIN_WIDTH / 2.0
		- (bounds.min_x + bounds.max_x) * projection.scale / 2.0;
	projection.offset_y = WIN_HEIGHT / 2.0
		- (bounds.min_y + bounds.max_y) * projection.scale / 2.0;
	return (projection);
}

t_point	rotation_point(t_point point, t_rotation *rotation,
		t_projection *projection)
{
	t_projected	projected;

	projected = rotate_project(point, &rotation->info.map, rotation->angle);
	point.x = round(projected.x * projection->scale + projection->offset_x);
	point.y = round(projected.y * projection->scale + projection->offset_y);
	return (point);
}
