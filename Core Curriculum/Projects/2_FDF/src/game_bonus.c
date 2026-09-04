/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 00:00:00 by hnah              #+#    #+#             */
/*   Updated: 2026/09/04 00:00:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game_bonus.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

int	game_input_init(t_game_input *input, int fd)
{
	int	flags;

	if (input == NULL || fd < 0)
		return (GAME_INPUT_ERROR);
	flags = fcntl(fd, F_GETFL);
	if (flags == -1)
		return (GAME_INPUT_ERROR);
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		return (GAME_INPUT_ERROR);
	input->fd = fd;
	input->original_flags = flags;
	input->active = 1;
	return (GAME_INPUT_NONE);
}

int	game_input_read_key(t_game_input *input, char *key)
{
	ssize_t	bytes_read;

	if (input == NULL || key == NULL || input->active == 0)
		return (GAME_INPUT_ERROR);
	bytes_read = read(input->fd, key, 1);
	if (bytes_read == 1)
		return (GAME_INPUT_KEY);
	if (bytes_read == 0)
	{
		input->active = 0;
		return (GAME_INPUT_CLOSED);
	}
	if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
		return (GAME_INPUT_NONE);
	return (GAME_INPUT_ERROR);
}

int	game_input_restore(t_game_input *input)
{
	if (input == NULL)
		return (GAME_INPUT_ERROR);
	if (fcntl(input->fd, F_SETFL, input->original_flags) == -1)
		return (GAME_INPUT_ERROR);
	return (GAME_INPUT_NONE);
}

int	game_input_hook(void *parameter)
{
	t_game_input	*input;
	char			key;
	int				status;

	input = parameter;
	if (input->active == 0)
		return (0);
	status = game_input_read_key(input, &key);
	while (status == GAME_INPUT_KEY)
	{
		if (key == 'w' || key == 'a' || key == 's' || key == 'd'
			|| key == 'W' || key == 'A' || key == 'S' || key == 'D')
		{
			write(STDOUT_FILENO, &key, 1);
			write(STDOUT_FILENO, "\n", 1);
		}
		status = game_input_read_key(input, &key);
	}
	return (0);
}
