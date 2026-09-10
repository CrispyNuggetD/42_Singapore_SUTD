/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   perspective_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/10 00:00:00 by hnah              #+#    #+#             */
/*   Updated: 2026/09/10 00:00:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rotation_bonus.h"

static t_camera_point	camera_point(t_point point, t_camera *camera)
{
	t_camera_point	result;
	double			dx;
	double			dy;

	dx = point.x - camera->x;
	dy = point.y - camera->y;
	result.right = -sin(camera->yaw) * dx + cos(camera->yaw) * dy;
	result.depth = cos(camera->yaw) * dx + sin(camera->yaw) * dy;
	result.vertical = point.z - camera->z;
	result.colour = point.colour;
	return (result);
}

static void	draw_perspective_edge(t_rotation *rotation,
		t_point first, t_point second)
{
	t_camera_point	camera_first;
	t_camera_point	camera_second;
	t_point			screen_first;
	t_point			screen_second;

	camera_first = camera_point(first, &rotation->camera);
	camera_second = camera_point(second, &rotation->camera);
	if (camera_first.depth <= rotation->camera.near_plane
		|| camera_second.depth <= rotation->camera.near_plane)
		return ;
	screen_first.x = WIN_WIDTH / 2 + rotation->camera.focal
		* camera_first.right / camera_first.depth;
	screen_first.y = WIN_HEIGHT / 2 - rotation->camera.focal
		* camera_first.vertical / camera_first.depth;
	screen_first.colour = camera_first.colour;
	screen_second.x = WIN_WIDTH / 2 + rotation->camera.focal
		* camera_second.right / camera_second.depth;
	screen_second.y = WIN_HEIGHT / 2 - rotation->camera.focal
		* camera_second.vertical / camera_second.depth;
	screen_second.colour = camera_second.colour;
	draw_line(&rotation->info.image, screen_first, screen_second);
}

static void	draw_neighbours(t_rotation *rotation, int index)
{
	t_point	*points;
	int		width;

	points = rotation->info.map.points;
	width = rotation->info.map.width;
	if (points[index].x + 1 < width)
		draw_perspective_edge(rotation, points[index], points[index + 1]);
	if (points[index].y + 1 < rotation->info.map.height)
		draw_perspective_edge(rotation, points[index], points[index + width]);
}

static void	update_camera_position(t_rotation *rotation)
{
	t_point	point;
	int		index;

	index = rotation->players[HOST_PLAYER].y * rotation->info.map.width
		+ rotation->players[HOST_PLAYER].x;
	point = rotation->info.map.points[index];
	rotation->camera.x = point.x;
	rotation->camera.y = point.y;
	rotation->camera.z = point.z + CAMERA_EYE_HEIGHT;
}

void	perspective_render(t_rotation *rotation)
{
	t_point	player_base;
	t_point	player_top;
	int		index;

	update_camera_position(rotation);
	index = 0;
	while (index < rotation->info.image.line_length * WIN_HEIGHT)
		rotation->info.image.addr[index++] = 0;
	index = 0;
	while (index < rotation->info.map.width * rotation->info.map.height)
		draw_neighbours(rotation, index++);
	index = rotation->players[REMOTE_PLAYER].y * rotation->info.map.width
		+ rotation->players[REMOTE_PLAYER].x;
	player_base = rotation->info.map.points[index];
	player_base.colour = REMOTE_COLOUR;
	player_top = player_base;
	player_top.z += PLAYER_SIZE;
	draw_perspective_edge(rotation, player_base, player_top);
	mlx_put_image_to_window(rotation->info.mlx, rotation->info.win,
		rotation->info.image.ptr, 0, 0);
}
