/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 13:34:07 by hnah              #+#    #+#             */
/*   Updated: 2026/09/11 15:23:59 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rotation_bonus.h"
#include <sys/select.h>
#include <sys/time.h>

long	rotation_time_us(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000000L + time.tv_usec);
}

static int	read_remote_key(char *key)
{
	struct timeval	timeout;
	fd_set			read_fds;
	int				ready;

	FD_ZERO(&read_fds);
	FD_SET(STDIN_FILENO, &read_fds);
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	ready = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);
	if (ready <= 0)
		return (ready);
	if (read(STDIN_FILENO, key, 1) == 1)
		return (1);
	return (-1);
}

int	game_move_player(t_rotation *rotation, int player_index, char key)
{
	t_player	*player;
	int			new_x;
	int			new_y;

	player = &rotation->players[player_index];
	new_x = player->x;
	new_y = player->y;
	if (key == 'w' || key == 'W')
		new_y--;
	else if (key == 's' || key == 'S')
		new_y++;
	else if (key == 'a' || key == 'A')
		new_x--;
	else if (key == 'd' || key == 'D')
		new_x++;
	if (new_x < 0 || new_x >= rotation->info.map.width
		|| new_y < 0 || new_y >= rotation->info.map.height
		|| (new_x == player->x && new_y == player->y))
		return (0);
	player->x = new_x;
	player->y = new_y;
	return (1);
}

static int	remote_action(t_rotation *rotation, char key)
{
	if (key == 'q' || key == 'Q')
	{
		rotation->cameras[REMOTE_PLAYER].yaw -= REMOTE_TURN_STEP;
		return (1);
	}
	if (key == 'e' || key == 'E')
	{
		rotation->cameras[REMOTE_PLAYER].yaw += REMOTE_TURN_STEP;
		return (1);
	}
	return (game_move_player(rotation, REMOTE_PLAYER, key));
}

int	game_input_update(t_rotation *rotation)
{
	char	key;
	int		status;
	int		moved;

	if (rotation->remote_active == 0)
		return (0);
	moved = 0;
	status = read_remote_key(&key);
	while (status > 0)
	{
		moved |= remote_action(rotation, key);
		status = read_remote_key(&key);
	}
	if (status < 0)
		rotation->remote_active = 0;
	return (moved);
}
