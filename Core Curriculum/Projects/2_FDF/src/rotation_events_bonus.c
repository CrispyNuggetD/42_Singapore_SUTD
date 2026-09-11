/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rotation_events_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 00:00:00 by hnah              #+#    #+#             */
/*   Updated: 2026/09/04 00:00:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rotation_bonus.h"

static int	move_host_player(t_rotation *rotation, int keycode)
{
	if (keycode == KEY_W)
		return (game_move_player(rotation, HOST_PLAYER, 'w'));
	if (keycode == KEY_S)
		return (game_move_player(rotation, HOST_PLAYER, 's'));
	if (keycode == KEY_A)
		return (game_move_player(rotation, HOST_PLAYER, 'a'));
	if (keycode == KEY_D)
		return (game_move_player(rotation, HOST_PLAYER, 'd'));
	return (0);
}

int	rotation_key_press(int keycode, void *parameter)
{
	t_rotation	*rotation;

	rotation = parameter;
	if (keycode == KEY_ESC)
		game_close(rotation);
	else if (keycode == KEY_V)
		game_toggle_view(rotation);
	else if (keycode == KEY_LEFT)
		rotation->direction = -1;
	else if (keycode == KEY_RIGHT)
		rotation->direction = 1;
	else if (move_host_player(rotation, keycode))
		game_render(rotation);
	else if (game_adjust_view(rotation, keycode))
		game_render(rotation);
	return (0);
}

int	rotation_key_release(int keycode, void *parameter)
{
	t_rotation	*rotation;

	rotation = parameter;
	if (keycode == KEY_LEFT || keycode == KEY_RIGHT)
		rotation->direction = 0;
	return (0);
}

static int	rotation_update(t_rotation *rotation, long now)
{
	double		seconds;

	if (now - rotation->last_frame < FRAME_US)
		return (0);
	seconds = (now - rotation->last_frame) / 1000000.0;
	if (seconds > 0.05)
		seconds = 0.05;
	rotation->last_frame = now;
	if (rotation->direction == 0)
		return (0);
	if (rotation->view_mode == VIEW_ISOMETRIC)
		rotation->angle += rotation->direction * ROTATION_SPEED * seconds;
	else
		rotation->cameras[HOST_PLAYER].yaw += rotation->direction
			* ROTATION_SPEED * seconds;
	return (1);
}

int	rotation_loop(void *parameter)
{
	t_rotation	*rotation;
	int			redraw;

	rotation = parameter;
	redraw = game_input_update(rotation);
	redraw |= rotation_update(rotation, rotation_time_us());
	if (redraw != 0)
		game_render(rotation);
	return (0);
}
