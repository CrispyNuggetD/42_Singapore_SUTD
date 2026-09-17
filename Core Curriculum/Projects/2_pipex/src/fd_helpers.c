/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd_helpers.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 13:57:17 by hnah              #+#    #+#             */
/*   Updated: 2026/09/17 17:54:52 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	init_pipeline(t_pipeline *pipeline)
{
	pipeline->input_fd = -1;
	pipeline->output_fd = -1;
	pipeline->pipefd[0] = -1;
	pipeline->pipefd[1] = -1;
	pipeline->child_count = 0;
	pipeline->last_pid = -1;
}

int open_output(const char *path, int mode)
{
	if (mode != O_APPEND && mode != O_TRUNC)
	{
		errno = EINVAL;
		return (-1);
	}
	return (open(path, O_WRONLY | O_CREAT | mode, 0666));
}

int	open_files_no_heredoc(t_pipeline *pipeline, int argc, char **argv)
{
	pipeline->first_command = 2;
	pipeline->last_command = argc - 2;
	pipeline->input_fd = open(argv[1], O_RDONLY);
	if (pipeline->input_fd < 0)
		perror(argv[1]);
	pipeline->output_fd = open_output(argv[argc - 1], O_TRUNC);
	if (pipeline->output_fd < 0)
		return (return_perror(argv[argc - 1]));
	return (0);
}

void	close_and_void_fd(int *fd)
{
	if (*fd >= 0)
	{
		close(*fd);
		*fd = -1;
	}
}

int	close_pipeline(t_pipeline *pipeline)
{
	if (pipeline->input_fd >= 0)
		close_and_void_fd(&pipeline->input_fd);
	if (pipeline->output_fd >= 0)
		close_and_void_fd(&pipeline->output_fd);
	if (pipeline->pipefd[0] >= 0)
		close_and_void_fd(&pipeline->pipefd[0]);
	if (pipeline->pipefd[1] >= 0)
		close_and_void_fd(&pipeline->pipefd[1]);
	return (1);
}