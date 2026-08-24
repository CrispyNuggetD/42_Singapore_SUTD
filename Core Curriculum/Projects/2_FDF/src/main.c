/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 03:27:00 by hnah              #+#    #+#             */
/*   Updated: 2026/08/25 03:27:00 by hnah             ###   ########.fr       */
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
	t_fdf	fdf;

	if (argc != 2)
		return (print_usage());
	if (fdf_init(&fdf, argv[1]) != 0)
	{
		fdf_destroy(&fdf);
		write(2, "Error: MiniLibX initialization failed\n", 38);
		return (1);
	}
	mlx_put_image_to_window(fdf.mlx, fdf.win, fdf.image.ptr, 0, 0);
	mlx_hook(fdf.win, EVENT_KEY_PRESS, 0, fdf_key_press, &fdf);
	mlx_hook(fdf.win, EVENT_DESTROY, 0, fdf_close, &fdf);
	mlx_loop(fdf.mlx);
	return (0);
}
