/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_render_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/10 00:00:00 by hnah              #+#    #+#             */
/*   Updated: 2026/09/10 00:00:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rotation_bonus.h"
#include "projection.h"

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
	projection = render_map(&rotation->info, rotation->angle);
	draw_player(rotation, &projection, HOST_PLAYER, HOST_COLOUR);
	draw_player(rotation, &projection, REMOTE_PLAYER, REMOTE_COLOUR);
	mlx_put_image_to_window(rotation->info.mlx, rotation->info.win,
		rotation->info.image.ptr, 0, 0);
}
