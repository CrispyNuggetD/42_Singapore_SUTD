/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 19:05:09 by hnah              #+#    #+#             */
/*   Updated: 2026/09/25 20:47:42 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include "../libft/ryker_libft.h"
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
# include <errno.h>

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

typedef enum e_path_result
{
	ALLOCATION_FAILED = -1,
	COMMAND_NOT_FOUND = 0,
	DIRECT_PATH_SUPPLIED = 1,
	EXEC_FAILED,
	SEARCH_CONTINUE
}	t_path_result;

void			init_pipeline(t_pipeline *pipeline);
int				open_output(const char *path, int mode);
int				open_files_no_heredoc(t_pipeline *pipeline, int argc,
					char **argv);
void			close_and_void_fd(int *fd);
int				close_pipeline(t_pipeline *pipeline);
int				run_pipeline(t_pipeline *pipeline, char **argv, char **envp);
char			**split_path_empty_as_dot(char *path);
char			**split_command_args(const char *command);
void			execute_command(char *command_str, char **envp);
t_path_result	attempt_possible_candidates(char **args, char **envp,
					char **exec_fail_path);
t_path_result	attempt_one_candidate(char *candidate_command, char **args,
					char **envp, char **exec_fail_path);
void			exit_perror(char *message, int status);
int				return_perror(char *message);
int				print_usage(t_usage_message_id message_id);

#endif
