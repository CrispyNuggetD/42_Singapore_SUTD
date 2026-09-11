/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 13:34:07 by hnah              #+#    #+#             */
/*   Updated: 2026/09/11 15:23:59 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rotation_bonus.h"

static int	print_usage(void)
{
	write(2, "Usage: ./fdf <map.fdf>\n", 23);
	return (1);
}

static int	load_bonus(t_rotation *rotation, char *filename)
{
	if (fdf_init(&rotation->info, filename) != 0)
	{
		fdf_destroy(&rotation->info);
		write(2, "Error: MiniLibX initialization failed\n", 38);
		return (1);
	}
	if (read_map(filename, &rotation->info.map) != 0)
	{
		fdf_destroy(&rotation->info);
		write(2, "Error: invalid map\n", 19);
		return (1);
	}
	return (0);
}

static void	init_cameras(t_rotation *rotation)
{
	double	centre_x;
	double	centre_y;

	centre_x = (rotation->info.map.width - 1) / 2.0;
	centre_y = (rotation->info.map.height - 1) / 2.0;
	rotation->cameras[HOST_PLAYER].yaw = atan2(centre_y, centre_x);
	rotation->cameras[REMOTE_PLAYER].yaw = atan2(centre_y
			- rotation->players[REMOTE_PLAYER].y, centre_x
			- rotation->players[REMOTE_PLAYER].x);
	rotation->cameras[HOST_PLAYER].focal = WIN_WIDTH
		/ (2.0 * tan(CAMERA_FOV / 2.0));
	rotation->cameras[REMOTE_PLAYER].focal
		= rotation->cameras[HOST_PLAYER].focal;
	rotation->cameras[HOST_PLAYER].near_plane = CAMERA_NEAR;
	rotation->cameras[REMOTE_PLAYER].near_plane = CAMERA_NEAR;
}

static void	init_bonus_state(t_rotation *rotation)
{
	rotation->angle = 0.0;
	rotation->zoom = 1.0;
	rotation->shift.x = 0.0;
	rotation->shift.y = 0.0;
	rotation->direction = 0;
	rotation->remote_active = 1;
	rotation->view_mode = VIEW_ISOMETRIC;
	rotation->players[HOST_PLAYER].x = 0;
	rotation->players[HOST_PLAYER].y = 0;
	rotation->players[REMOTE_PLAYER].x = rotation->info.map.width - 1;
	rotation->players[REMOTE_PLAYER].y = rotation->info.map.height - 1;
	rotation->remote_view.win = NULL;
	rotation->remote_view.image.ptr = NULL;
	init_cameras(rotation);
	rotation->last_frame = rotation_time_us();
}

int	main(int argc, char **argv)
{
	t_rotation	rotation;

	if (argc != 2)
		return (print_usage());
	if (load_bonus(&rotation, argv[1]) != 0)
		return (1);
	init_bonus_state(&rotation);
	game_render(&rotation);
	mlx_hook(rotation.info.win, EVENT_KEY_PRESS, MASK_KEY_PRESS,
		rotation_key_press, &rotation);
	mlx_hook(rotation.info.win, EVENT_KEY_RELEASE, MASK_KEY_RELEASE,
		rotation_key_release, &rotation);
	mlx_hook(rotation.info.win, EVENT_DESTROY, 0, game_close, &rotation);
	mlx_loop_hook(rotation.info.mlx, rotation_loop, &rotation);
	mlx_loop(rotation.info.mlx);
	return (0);
}
