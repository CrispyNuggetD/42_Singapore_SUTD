/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 21:05:36 by hnah              #+#    #+#             */
/*   Updated: 2026/09/11 21:05:36 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

static int	line_error(char **line)
{
	free(*line);
	*line = NULL;
	return (-1);
}

static int	grow_line(char **line, size_t *capacity, size_t length)
{
	char	*larger;

	*capacity *= 2;
	larger = malloc(*capacity);
	if (!larger)
		return (-1);
	ft_memcpy(larger, *line, length);
	free(*line);
	*line = larger;
	return (0);
}

static int	finish_line(char **line, size_t length, ssize_t bytes)
{
	if (bytes == 0 && length == 0)
	{
		free(*line);
		*line = NULL;
		return (0);
	}
	(*line)[length] = '\0';
	return (1);
}

int	read_line_bonus(int fd, char **line)
{
	size_t	length;
	size_t	capacity;
	ssize_t	bytes;

	length = 0;
	capacity = 128;
	*line = malloc(capacity);
	if (!*line)
		return (-1);
	bytes = 1;
	while (bytes > 0)
	{
		if (length + 1 >= capacity
			&& grow_line(line, &capacity, length) < 0)
			return (line_error(line));
		bytes = read(fd, *line + length, 1);
		if (bytes < 0)
			return (line_error(line));
		if (bytes > 0 && (*line)[length++] == '\n')
			break ;
	}
	return (finish_line(line, length, bytes));
}
