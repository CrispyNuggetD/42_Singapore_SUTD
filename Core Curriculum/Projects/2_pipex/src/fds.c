/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fds.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 00:00:00 by hnah              #+#    #+#             */
/*   Updated: 2026/08/27 00:00:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	fds_init(t_fds *fds)
{
	fds->infile = -1;
	fds->outfile = -1;
	fds->pipefd[0] = -1;
	fds->pipefd[1] = -1;
}

int	fds_open(t_fds *fds, char **argv)
{
	fds->infile = open(argv[1], O_RDONLY);
	if (fds->infile < 0)
		perror(argv[1]);
	fds->outfile = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fds->outfile < 0)
		return (perror(argv[4]), 1);
	if (pipe(fds->pipefd) < 0)
		return (perror("pipe"), 1);
	return (0);
}

static void	close_one(int *fd)
{
	if (*fd >= 0)
	{
		close(*fd);
		*fd = -1;
	}
}

void	fds_close(t_fds *fds)
{
	close_one(&fds->infile);
	close_one(&fds->outfile);
	close_one(&fds->pipefd[0]);
	close_one(&fds->pipefd[1]);
}
