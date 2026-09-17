/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 19:05:09 by hnah              #+#    #+#             */
/*   Updated: 2026/09/17 15:26:06 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include "../ryker_libft/ryker_libft.h"
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
# include <errno.h>


/* 
typedef struct s_fds
{
	int	infile;
	int	outfile;
	int	pipefd[2];
}	t_fds; */
//REMOVE LATER AFT DONE [1/?]

typedef struct s_pipeline
{
	int		input_fd;
	int		output_fd;
	int		pipefd[2];






	int		first_command;
	int		last_command;
	int		child_count;
	pid_t	last_pid;
}	t_pipeline;

typedef enum e_usage_message_id
{
	USAGE_MANDATORY,
	USAGE_BONUS,
	USAGE_HEREDOC,
}	t_usage_message_id;

void	init_pipeline(t_pipeline *pipeline);
void	fds_close(t_fds *fds);
int		spawn_children(t_fds *fds, char **argv, char **envp);
void	execute_command(char *command, char **envp);
char	*resolve_path(char *command, char **envp);
void	free_matrix(char **matrix);
void	exit_perror(char *message, int status);
int		return_perror(char *message);
int		print_usage(t_usage_message_id message_id)


#endif
