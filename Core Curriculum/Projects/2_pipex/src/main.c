/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 00:00:00 by hnah              #+#    #+#             */
/*   Updated: 2026/08/27 00:00:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	main(int argc, char **argv, char **envp)
{
	t_fds	fds;
	int		status;

	if (argc != 5)
	{
		write(2, "Usage: ./pipex infile cmd1 cmd2 outfile\n", 42);
		return (1);
	}
	fds_init(&fds);
	if (fds_open(&fds, argv) != 0)
	{
		fds_close(&fds);
		return (1);
	}
	status = spawn_children(&fds, argv, envp);
	fds_close(&fds);
	return (status);
}
