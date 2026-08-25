/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 03:27:00 by hnah              #+#    #+#             */
/*   Updated: 2026/08/26 01:06:41 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

static int	print_usage(void)
{
	write(2, "Usage: ./fdf <map.fdf>\n", 23);
	return (1);
}

int	main(int argc, char **argv)
{
	t_info	info;

	if (argc != 2)
		return (print_usage());
	if (fdf_init(&info, argv[1]) != 0)
	{
		fdf_destroy(&info);
		write(2, "Error: MiniLibX initialization failed\n", 38);
		return (1);
	}
	if (read_map(argv[1], &info.map) != 0)
	{
		fdf_destroy(&info);
		write(2, "Error: invalid map\n", 19);
		return (1);
	}
	fdf_put_pixel(&info.image, WIN_WIDTH / 2, WIN_HEIGHT / 2, 0x00FFFFFF);
	mlx_put_image_to_window(info.mlx, info.win, info.image.ptr, 0, 0);
	mlx_hook(info.win, EVENT_KEY_PRESS, 0, fdf_key_press, &info);
	mlx_hook(info.win, EVENT_DESTROY, 0, fdf_close, &info);
	mlx_loop(info.mlx);
	return (0);
}
