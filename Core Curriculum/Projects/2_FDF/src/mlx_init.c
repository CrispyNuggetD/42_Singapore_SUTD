/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 03:27:00 by hnah              #+#    #+#             */
/*   Updated: 2026/08/26 00:08:17 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

static void	init_info(t_info *info)
{
	info->mlx = NULL;
	info->win = NULL;
	info->image.ptr = NULL;
	info->image.addr = NULL;
	info->image.bytes_per_pixel = 0;
	info->map.width = 0;
	info->map.height = 0;
	info->map.points = NULL;
}

int	fdf_init(t_info *info, char *title)
{
	init_info(info);
	info->mlx = mlx_init();
	if (info->mlx == NULL)
		return (1);
	info->win = mlx_new_window(info->mlx, WIN_WIDTH, WIN_HEIGHT, title);
	if (info->win == NULL)
		return (1);
	info->image.ptr = mlx_new_image(info->mlx, WIN_WIDTH, WIN_HEIGHT);
	if (info->image.ptr == NULL)
		return (1);
	info->image.addr = mlx_get_data_addr(info->image.ptr,
			&info->image.bits_per_pixel, &info->image.line_length,
			&info->image.endian);
	if (info->image.addr == NULL)
		return (1);
	info->image.bytes_per_pixel = info->image.bits_per_pixel / 8;
	return (0);
}

void	fdf_destroy(t_info *info)
{
	free(info->map.points);
	if (info->image.ptr != NULL)
		mlx_destroy_image(info->mlx, info->image.ptr);
	if (info->win != NULL)
		mlx_destroy_window(info->mlx, info->win);
	if (info->mlx != NULL)
	{
#ifndef __APPLE__
		mlx_destroy_display(info->mlx);
		free(info->mlx);
#endif
	}
}
