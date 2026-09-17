/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 21:05:36 by hnah              #+#    #+#             */
/*   Updated: 2026/09/18 05:07:50 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

static int	tmp_file_error(char *unique_filepath, int *heredoc_write_fd)
{
	perror("here_doc");
	close_and_void_fd(heredoc_write_fd);
	if (unique_filepath)
	{
		unlink(unique_filepath);
		free(unique_filepath);
	}
	return (-1);
}

static int	collect_heredoc_lines(int heredoc_write_fd, char *limiter)
{
	char			*line;
	t_gnl_info		gnl;
	t_gnl_result	result;

	ryker_ft_gnl_init(&gnl, STDIN_FILENO);
	result = ryker_ft_get_next_line(&gnl, &line);
	while (result == GNL_LINE && !is_limiter(line, limiter))
	{
		if (write_line_guaranteed(heredoc_write_fd, line) < 0)
			return (heredoc_cleanup(&gnl, line, -1));
		free(line);
		result = ryker_ft_get_next_line(&gnl, &line);
	}
	if (result == GNL_ERROR)
		return (heredoc_cleanup(&gnl, line, -1));
	return (heredoc_cleanup(&gnl, line, 0));
}

int	prepare_heredoc(char *limiter)
{
	char	*unique_filepath;
	int		heredoc_write_fd;
	int		read_fd;

	heredoc_write_fd = ryker_ft_create_open_unique_file(&unique_filepath,
			"/tmp/pipex_here_doc_");
	if (heredoc_write_fd < 0
		|| collect_heredoc_lines(heredoc_write_fd, limiter) < 0)
		return (tmp_file_error(unique_filepath, &heredoc_write_fd));
	close_and_void_fd(&heredoc_write_fd);
	read_fd = open(unique_filepath, O_RDONLY);
	if (read_fd < 0)
		return (tmp_file_error(unique_filepath, &heredoc_write_fd));
	unlink(unique_filepath);
	free(unique_filepath);
	return (read_fd);
}
