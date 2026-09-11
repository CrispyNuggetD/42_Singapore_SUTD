/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_render_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/10 18:25:42 by hnah              #+#    #+#             */
/*   Updated: 2026/09/11 15:23:59 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rotation_bonus.h"
#include "projection.h"

int	game_close(void *parameter)
{
	game_destroy(parameter);
	exit(0);
	return (0);
}

void	game_update_camera(t_rotation *rotation, int player)
{
	t_point	point;
	int		index;

	index = rotation->players[player].y * rotation->info.map.width
		+ rotation->players[player].x;
	point = rotation->info.map.points[index];
	rotation->cameras[player].x = point.x;
	rotation->cameras[player].y = point.y;
	rotation->cameras[player].z = point.z + CAMERA_EYE_HEIGHT;
}

int	game_adjust_view(t_rotation *rotation, int keycode)
{
	if (rotation->view_mode != VIEW_ISOMETRIC)
		return (0);
	if (keycode == KEY_EQUAL && rotation->zoom < ZOOM_MAX)
		rotation->zoom *= ZOOM_STEP;
	else if (keycode == KEY_MINUS && rotation->zoom > ZOOM_MIN)
		rotation->zoom /= ZOOM_STEP;
	else if (keycode == KEY_Z || keycode == KEY_KP_4)
		rotation->shift.x -= TRANSLATE_STEP;
	else if (keycode == KEY_C || keycode == KEY_KP_6)
		rotation->shift.x += TRANSLATE_STEP;
	else if (keycode == KEY_R || keycode == KEY_KP_8)
		rotation->shift.y -= TRANSLATE_STEP;
	else if (keycode == KEY_F || keycode == KEY_KP_5)
		rotation->shift.y += TRANSLATE_STEP;
	else
		return (0);
	return (1);
}

static void	draw_player(t_rotation *rotation, t_projection *projection,
		int player, int colour)
{
	t_point	centre;
	t_point	start;
	t_point	end;
	int		index;

	index = rotation->players[player].y * rotation->info.map.width
		+ rotation->players[player].x;
	centre = project_point(rotation->info.map.points[index],
			&rotation->info.map, projection);
	centre.colour = colour;
	start = centre;
	end = centre;
	start.x -= PLAYER_SIZE;
	end.x += PLAYER_SIZE;
	draw_line(&rotation->info.image, start, end);
	start = centre;
	end = centre;
	start.y -= PLAYER_SIZE;
	end.y += PLAYER_SIZE;
	draw_line(&rotation->info.image, start, end);
}

void	game_render(t_rotation *rotation)
{
	t_projection	projection;

	if (rotation->view_mode == VIEW_FIRST_PERSON)
	{
		perspective_render(rotation, HOST_PLAYER, &rotation->info.image,
			rotation->info.win);
		perspective_render(rotation, REMOTE_PLAYER,
			&rotation->remote_view.image, rotation->remote_view.win);
		return ;
	}
	projection = render_map_view(&rotation->info, rotation->angle,
			rotation->zoom, rotation->shift);
	draw_player(rotation, &projection, HOST_PLAYER, HOST_COLOUR);
	draw_player(rotation, &projection, REMOTE_PLAYER, REMOTE_COLOUR);
	mlx_put_image_to_window(rotation->info.mlx, rotation->info.win,
		rotation->info.image.ptr, 0, 0);
}
