/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 21:05:36 by hnah              #+#    #+#             */
/*   Updated: 2026/09/17 23:08:59 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

static int	temp_error(char *name, int fd)
{
	perror("here_doc");
	if (fd >= 0)
		close(fd);
	if (name)
	{
		unlink(name);
		free(name);
	}
	return (-1);
}

static char	*temp_name(int number)
{
	char	*digits;
	char	*name;

	digits = ft_itoa(number);
	if (!digits)
		return (NULL);
	name = ft_strjoin("/tmp/pipex_here_doc_", digits);
	free(digits);
	return (name);
}

static int	create_temp(char **name)
{
	int	fd;
	int	number;

	number = 0;
	while (number >= 0)
	{
		*name = temp_name(number++);
		if (!*name)
			return (-1);
		fd = open(*name, O_WRONLY | O_CREAT | O_EXCL, 0600);
		if (fd >= 0 || errno != EEXIST)
			return (fd);
		free(*name);
	}
	return (-1);
}

static int	collect_lines(int fd, char *limiter)
{
	char	*line;
	int		result;

	result = read_line_bonus(STDIN_FILENO, &line);
	while (result > 0 && !is_limiter_bonus(line, limiter))
	{
		if (write_line_bonus(fd, line) < 0)
		{
			free(line);
			return (-1);
		}
		free(line);
		result = read_line_bonus(STDIN_FILENO, &line);
	}
	free(line);
	if (result < 0)
		return (-1);
	return (0);
}

int	prepare_heredoc_bonus(char *limiter)
{
	char	*name;
	int		write_fd;
	int		read_fd;

	write_fd = create_temp(&name);
	if (write_fd < 0)
		return (temp_error(name, -1));
	if (collect_lines(write_fd, limiter) < 0)
		return (temp_error(name, write_fd));
	close(write_fd);
	read_fd = open(name, O_RDONLY);
	unlink(name);
	free(name);
	if (read_fd < 0)
		perror("here_doc");
	return (read_fd);
}
