/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   projection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 02:51:15 by hnah              #+#    #+#             */
/*   Updated: 2026/08/26 05:23:49 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

static t_projected	project_model(t_point point, t_map *map,
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

static void	update_bounds(t_bounds *bounds, t_projected point)
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

static t_bounds	find_bounds(t_map *map, t_projection *projection)
{
	t_bounds		bounds;
	t_projected	point;
	int				index;

	point = project_model(map->points[0], map, projection);
	bounds.min_x = point.x;
	bounds.max_x = point.x;
	bounds.min_y = point.y;
	bounds.max_y = point.y;
	index = 1;
	while (index < map->width * map->height)
	{
		point = project_model(map->points[index], map, projection);
		update_bounds(&bounds, point);
		index++;
	}
	return (bounds);
}

static void	fit_projection(t_projection *projection, t_bounds *bounds)
{
	double	width;
	double	height;
	double	vertical_scale;

	width = bounds->max_x - bounds->min_x;
	height = bounds->max_y - bounds->min_y;
	projection->scale = 1.0;
	if (width > 0.0)
		projection->scale = (WIN_WIDTH - WINDOW_MARGIN) / width;
	if (height > 0.0)
	{
		vertical_scale = (WIN_HEIGHT - WINDOW_MARGIN) / height;
		if (width == 0.0 || vertical_scale < projection->scale)
			projection->scale = vertical_scale;
	}
	projection->offset_x = WIN_WIDTH / 2.0
		- (bounds->min_x + bounds->max_x) * projection->scale / 2.0;
	projection->offset_y = WIN_HEIGHT / 2.0
		- (bounds->min_y + bounds->max_y) * projection->scale / 2.0;
}

static t_projection	init_projection(t_map *map)
{
	t_projection	projection;
	t_bounds		bounds;

	projection.cos_angle = cos(ISO_ANGLE);
	projection.sin_angle = sin(ISO_ANGLE);
	bounds = find_bounds(map, &projection);
	fit_projection(&projection, &bounds);
	return (projection);
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
