/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 00:00:00 by hnah              #+#    #+#             */
/*   Updated: 2026/09/04 00:00:00 by hnah             ###   ########.fr       */
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

int	main(int argc, char **argv)
{
	t_rotation	rotation;

	if (argc != 2)
		return (print_usage());
	if (load_bonus(&rotation, argv[1]) != 0)
		return (1);
	rotation.angle = 0.0;
	rotation.direction = 0;
	rotation.last_frame = rotation_time_us();
	rotation_render(&rotation);
	mlx_hook(rotation.info.win, EVENT_KEY_PRESS, MASK_KEY_PRESS,
		rotation_key_press, &rotation);
	mlx_hook(rotation.info.win, EVENT_KEY_RELEASE, MASK_KEY_RELEASE,
		rotation_key_release, &rotation);
	mlx_hook(rotation.info.win, EVENT_DESTROY, 0, rotation_close, &rotation);
	mlx_loop_hook(rotation.info.mlx, rotation_loop, &rotation);
	mlx_loop(rotation.info.mlx);
	return (0);
}
