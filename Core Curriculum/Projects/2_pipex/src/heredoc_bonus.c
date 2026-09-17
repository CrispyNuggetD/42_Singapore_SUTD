/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 21:05:36 by hnah              #+#    #+#             */
/*   Updated: 2026/09/18 01:10:05 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

static int	temp_file_error(char *tmp_filename, int *fd)
{
	perror("here_doc");
	close_and_void_fd(fd);
	if (tmp_filename)
	{
		unlink(tmp_filename);
		free(tmp_filename);
	}
	return (-1);
}

static char	*generate_unique_filepath(char *base_path, int version_number)
{
	char	*digits;
	char	*unique_filepath;

	digits = ft_itoa(version_number);
	if (!digits)
		return (NULL);
	unique_filepath = ft_strjoin(base_path, digits);
	free(digits);
	return (unique_filepath);
}

static int	create_temp_file(char **tmp_filename)
{
	int	fd;
	int	version_number;

	version_number = 0;
	while (version_number >= 0)
	{
		*tmp_filename = generate_unique_filepath("/tmp/pipex_here_doc_",
				version_number++);
		if (!*tmp_filename)
			return (-1);
		fd = open(*tmp_filename, O_WRONLY | O_CREAT | O_EXCL, 0600);
		if (fd >= 0 || errno != EEXIST)
			return (fd);
		free(*tmp_filename);
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
	char	*tmp_filename;
	int		write_fd;
	int		read_fd;

	write_fd = create_temp_file(&tmp_filename);
	if (write_fd < 0 || collect_lines(write_fd, limiter) < 0)
		return (temp_file_error(tmp_filename, &write_fd));
	close_and_void_fd(&write_fd);
	read_fd = open(tmp_filename, O_RDONLY);
	unlink(tmp_filename);
	free(tmp_filename);
	if (read_fd < 0)
		perror("here_doc");
	return (read_fd);
}
