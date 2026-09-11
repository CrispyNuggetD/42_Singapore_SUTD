/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   perspective_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/10 18:25:42 by hnah              #+#    #+#             */
/*   Updated: 2026/09/11 15:23:59 by hnah             ###   ########.fr       */
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

static void	draw_perspective_edge(t_render_view *view,
		t_point first, t_point second)
{
	t_camera_point	camera_first;
	t_camera_point	camera_second;
	t_point			screen_first;
	t_point			screen_second;

	camera_first = camera_point(first, view->camera);
	camera_second = camera_point(second, view->camera);
	if (camera_first.depth <= view->camera->near_plane
		|| camera_second.depth <= view->camera->near_plane)
		return ;
	screen_first.x = WIN_WIDTH / 2 + view->camera->focal
		* camera_first.right / camera_first.depth;
	screen_first.y = WIN_HEIGHT / 2 - view->camera->focal
		* camera_first.vertical / camera_first.depth;
	screen_first.colour = camera_first.colour;
	screen_second.x = WIN_WIDTH / 2 + view->camera->focal
		* camera_second.right / camera_second.depth;
	screen_second.y = WIN_HEIGHT / 2 - view->camera->focal
		* camera_second.vertical / camera_second.depth;
	screen_second.colour = camera_second.colour;
	draw_line(view->image, screen_first, screen_second);
}

static void	draw_neighbours(t_rotation *rotation, t_render_view *view,
		int index)
{
	t_point	*points;
	int		width;

	points = rotation->info.map.points;
	width = rotation->info.map.width;
	if (points[index].x + 1 < width)
		draw_perspective_edge(view, points[index], points[index + 1]);
	if (points[index].y + 1 < rotation->info.map.height)
		draw_perspective_edge(view, points[index], points[index + width]);
}

static void	draw_other_player(t_rotation *rotation, t_render_view *view)
{
	t_point	player_base;
	t_point	player_top;
	int		player;
	int		index;

	player = 1 - view->player;
	index = rotation->players[player].y * rotation->info.map.width
		+ rotation->players[player].x;
	player_base = rotation->info.map.points[index];
	if (player == HOST_PLAYER)
		player_base.colour = HOST_COLOUR;
	else
		player_base.colour = REMOTE_COLOUR;
	player_top = player_base;
	player_top.z += PLAYER_SIZE;
	draw_perspective_edge(view, player_base, player_top);
}

void	perspective_render(t_rotation *rotation, int player,
		t_image *image, void *win)
{
	t_render_view	view;
	int				index;

	game_update_camera(rotation, player);
	view.camera = &rotation->cameras[player];
	view.image = image;
	view.player = player;
	index = 0;
	while (index < image->line_length * WIN_HEIGHT)
		image->addr[index++] = 0;
	index = 0;
	while (index < rotation->info.map.width * rotation->info.map.height)
		draw_neighbours(rotation, &view, index++);
	draw_other_player(rotation, &view);
	mlx_put_image_to_window(rotation->info.mlx, win, image->ptr, 0, 0);
}
