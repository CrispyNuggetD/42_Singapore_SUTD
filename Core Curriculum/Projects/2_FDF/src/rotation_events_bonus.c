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
#include <stdlib.h>
#include <sys/time.h>

long	rotation_time_us(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000000L + time.tv_usec);
}

int	rotation_close(void *parameter)
{
	t_rotation	*rotation;

	rotation = parameter;
	fdf_destroy(&rotation->info);
	exit(0);
	return (0);
}

int	rotation_key_press(int keycode, void *parameter)
{
	t_rotation	*rotation;

	rotation = parameter;
	if (keycode == KEY_ESC)
		rotation_close(rotation);
	else if (keycode == KEY_LEFT || keycode == KEY_A)
		rotation->direction = -1;
	else if (keycode == KEY_RIGHT || keycode == KEY_D)
		rotation->direction = 1;
	return (0);
}

int	rotation_key_release(int keycode, void *parameter)
{
	t_rotation	*rotation;

	rotation = parameter;
	if (keycode == KEY_LEFT || keycode == KEY_RIGHT
		|| keycode == KEY_A || keycode == KEY_D)
		rotation->direction = 0;
	return (0);
}

int	rotation_loop(void *parameter)
{
	t_rotation	*rotation;
	long		now;
	double		seconds;

	rotation = parameter;
	now = rotation_time_us();
	if (now - rotation->last_frame < FRAME_US)
		return (0);
	seconds = (now - rotation->last_frame) / 1000000.0;
	if (seconds > 0.05)
		seconds = 0.05;
	rotation->last_frame = now;
	if (rotation->direction != 0)
	{
		rotation->angle += rotation->direction * ROTATION_SPEED * seconds;
		rotation_render(rotation);
	}
	return (0);
}
