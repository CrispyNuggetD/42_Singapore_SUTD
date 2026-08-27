/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 19:05:09 by hnah              #+#    #+#             */
/*   Updated: 2026/08/27 19:22:31 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include "libft.h"
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

typedef struct s_fds
{
	int	infile;
	int	outfile;
	int	pipefd[2];
}	t_fds;

void	fds_init(t_fds *fds);
int		fds_open(t_fds *fds, char **argv);
void	fds_close(t_fds *fds);
int		spawn_children(t_fds *fds, char **argv, char **envp);
void	execute_command(char *command, char **envp);
char	*resolve_path(char *command, char **envp);
void	free_matrix(char **matrix);
void	exit_perror(char *message, int status);

#endif
