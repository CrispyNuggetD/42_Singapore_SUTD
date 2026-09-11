/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_window_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/10 18:41:32 by hnah              #+#    #+#             */
/*   Updated: 2026/09/11 15:23:59 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rotation_bonus.h"

static int	init_view_image(t_rotation *rotation, t_viewport *view)
{
	view->image.ptr = mlx_new_image(rotation->info.mlx, WIN_WIDTH, WIN_HEIGHT);
	if (view->image.ptr == NULL)
		return (1);
	view->image.addr = mlx_get_data_addr(view->image.ptr,
			&view->image.bits_per_pixel, &view->image.line_length,
			&view->image.endian);
	if (view->image.addr == NULL)
		return (1);
	view->image.bytes_per_pixel = view->image.bits_per_pixel / 8;
	return (0);
}

static int	open_remote_view(t_rotation *rotation)
{
	t_viewport	*view;

	view = &rotation->remote_view;
	view->win = mlx_new_window(rotation->info.mlx, WIN_WIDTH, WIN_HEIGHT,
			"FdF - Player 2");
	if (view->win == NULL || init_view_image(rotation, view) != 0)
		return (1);
	mlx_hook(view->win, EVENT_KEY_PRESS, MASK_KEY_PRESS,
		rotation_key_press, rotation);
	mlx_hook(view->win, EVENT_KEY_RELEASE, MASK_KEY_RELEASE,
		rotation_key_release, rotation);
	mlx_hook(view->win, EVENT_DESTROY, 0, game_close, rotation);
	return (0);
}

static void	close_remote_view(t_rotation *rotation)
{
	if (rotation->remote_view.image.ptr != NULL)
		mlx_destroy_image(rotation->info.mlx,
			rotation->remote_view.image.ptr);
	if (rotation->remote_view.win != NULL)
		mlx_destroy_window(rotation->info.mlx, rotation->remote_view.win);
	rotation->remote_view.image.ptr = NULL;
	rotation->remote_view.win = NULL;
}

void	game_destroy(t_rotation *rotation)
{
	close_remote_view(rotation);
	fdf_destroy(&rotation->info);
}

void	game_toggle_view(t_rotation *rotation)
{
	if (rotation->view_mode == VIEW_FIRST_PERSON)
	{
		close_remote_view(rotation);
		rotation->view_mode = VIEW_ISOMETRIC;
		rotation->direction = 0;
		game_render(rotation);
		return ;
	}
	if (open_remote_view(rotation) != 0)
	{
		write(2, "Error: second window initialization failed\n", 43);
		game_destroy(rotation);
		exit(1);
	}
	rotation->view_mode = VIEW_FIRST_PERSON;
	rotation->direction = 0;
	game_render(rotation);
}
