/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_and_errors.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 22:17:20 by hnah              #+#    #+#             */
/*   Updated: 2026/09/17 22:23:25 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	exit_perror(char *message, int status)
{
	perror(message);
	exit(status);
}

int	return_perror(char *message)
{
	perror(message);
	return (1);
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
	close_and_void_fd(&pipeline->input_fd);
	close_and_void_fd(&pipeline->output_fd);
	close_and_void_fd(&pipeline->pipefd[0]);
	close_and_void_fd(&pipeline->pipefd[1]);
	return (1);
}
