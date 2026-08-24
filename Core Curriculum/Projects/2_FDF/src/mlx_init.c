/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 03:27:00 by hnah              #+#    #+#             */
/*   Updated: 2026/08/25 03:27:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

static void	reset_fdf(t_fdf *fdf)
{
	fdf->mlx = NULL;
	fdf->win = NULL;
	fdf->image.ptr = NULL;
	fdf->image.addr = NULL;
}

int	fdf_init(t_fdf *fdf, char *title)
{
	reset_fdf(fdf);
	fdf->mlx = mlx_init();
	if (fdf->mlx == NULL)
		return (1);
	fdf->win = mlx_new_window(fdf->mlx, WIN_WIDTH, WIN_HEIGHT, title);
	if (fdf->win == NULL)
		return (1);
	fdf->image.ptr = mlx_new_image(fdf->mlx, WIN_WIDTH, WIN_HEIGHT);
	if (fdf->image.ptr == NULL)
		return (1);
	fdf->image.addr = mlx_get_data_addr(fdf->image.ptr,
			&fdf->image.bits_per_pixel, &fdf->image.line_length,
			&fdf->image.endian);
	if (fdf->image.addr == NULL)
		return (1);
	return (0);
}

void	fdf_destroy(t_fdf *fdf)
{
	if (fdf->image.ptr != NULL)
		mlx_destroy_image(fdf->mlx, fdf->image.ptr);
	if (fdf->win != NULL)
		mlx_destroy_window(fdf->mlx, fdf->win);
	if (fdf->mlx != NULL)
	{
		mlx_destroy_display(fdf->mlx);
		free(fdf->mlx);
	}
}
