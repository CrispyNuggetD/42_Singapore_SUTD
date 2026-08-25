/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   projection_bounds.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 05:46:00 by hnah              #+#    #+#             */
/*   Updated: 2026/08/26 05:50:57 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "projection.h"

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
	t_projected		point;
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

t_projection	init_projection(t_map *map)
{
	t_projection	projection;
	t_bounds		bounds;

	projection.cos_angle = cos(PI / 6.0);
	projection.sin_angle = sin(PI / 6.0);
	bounds = find_bounds(map, &projection);
	fit_projection(&projection, &bounds);
	return (projection);
}
