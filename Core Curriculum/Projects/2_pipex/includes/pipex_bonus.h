/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 21:05:36 by hnah              #+#    #+#             */
/*   Updated: 2026/09/11 21:05:36 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_BONUS_H
# define PIPEX_BONUS_H

# include "pipex.h"
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

int		read_line_bonus(int fd, char **line);
int		is_limiter_bonus(char *line, char *limiter);
int		write_line_bonus(int fd, char *line);
int		prepare_heredoc_bonus(char *limiter);
int		wait_pipeline_bonus(t_pipeline *pipeline);
int		run_pipeline_bonus(t_pipeline *pipeline, char **argv, char **envp);

#endif
