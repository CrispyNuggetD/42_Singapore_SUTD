/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   projection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 02:51:15 by hnah              #+#    #+#             */
/*   Updated: 2026/08/26 02:51:22 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

static t_projection	init_projection(t_map *map)
{
	t_projection	projection;
	int				span;
	int				vertical_scale;

	span = map->width + map->height;
	projection.scale = WIN_WIDTH / span;
	vertical_scale = WIN_HEIGHT / span;
	if (vertical_scale < projection.scale)
		projection.scale = vertical_scale;
	if (projection.scale < 1)
		projection.scale = 1;
	projection.cos_angle = cos(ISO_ANGLE);
	projection.sin_angle = sin(ISO_ANGLE);
	return (projection);
}

static t_point	project_point(t_point point, t_map *map,
		t_projection *projection)
{
	double	model_x;
	double	model_y;
	int		screen_x;
	int		screen_y;

	model_x = (point.x - (map->width - 1) / 2.0) * projection->scale;
	model_y = (point.y - (map->height - 1) / 2.0) * projection->scale;
	screen_x = (model_x - model_y) * projection->cos_angle;
	screen_y = (model_x + model_y) * projection->sin_angle;
	screen_y -= point.z * projection->scale;
	point.x = screen_x + WIN_WIDTH / 2;
	point.y = screen_y + WIN_HEIGHT / 2;
	return (point);
}

void	render_map(t_info *info)
{
	t_projection	projection;
	t_point			point;
	int				index;

	projection = init_projection(&info->map);
	index = 0;
	while (index < info->map.width * info->map.height)
	{
		point = project_point(info->map.points[index], &info->map,
				&projection);
		fdf_put_pixel(&info->image, point.x, point.y, 0x00FFFFFF);
		index++;
	}
}
