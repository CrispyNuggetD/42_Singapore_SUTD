/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 21:05:36 by hnah              #+#    #+#             */
/*   Updated: 2026/09/17 14:50:38 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"


static void	child_process(t_pipeline *pipeline, int command,
		char **argv, char **envp)
{
	if (pipeline->input_fd < 0)
		exit(1);
	if (dup2(pipeline->input_fd, STDIN_FILENO) < 0)
		exit_perror("dup2", 1);
	if (command == pipeline->last_command)
	{
		if (dup2(pipeline->output_fd, STDOUT_FILENO) < 0)
			exit_perror("dup2", 1);
	}
	else if (dup2(pipeline->pipefd[1], STDOUT_FILENO) < 0)
		exit_perror("dup2", 1);
	close_pipeline(pipeline);
	execute_command(argv[command], envp);
}

static void	advance_pipe(t_pipeline *pipeline, int command)
{
	if (pipeline->input_fd >= 0)
		close_and_void_fd(&pipeline->input_fd);
	pipeline->input_fd = -1;
	if (command < pipeline->last_command)
	{
		close_and_void_fd(&pipeline->pipefd[1]);
		pipeline->pipefd[1] = -1;
		pipeline->input_fd = pipeline->pipefd[0];
		pipeline->pipefd[0] = -1;
	}
}

static int	spawn_command(t_pipeline *pipeline, int command,
		char **argv, char **envp)
{
	pid_t	pid;

	if (command < pipeline->last_command && pipe(pipeline->pipefd) < 0)
		return (-1);
	pid = fork();
	if (pid < 0)
		return (-1);
	if (pid == 0)
		child_process(pipeline, command, argv, envp);
	pipeline->child_count++;
	pipeline->last_pid = pid;
	advance_pipe(pipeline, command);
	return (0);
}

int	run_pipeline_bonus(t_pipeline *pipeline, char **argv, char **envp)
{
	int	command;
	int	remaining;

	command = pipeline->first_command;
	while (command <= pipeline->last_command)
	{
		if (spawn_command(pipeline, command, argv, envp) < 0)
		{
			perror("pipex");
			close_pipeline(pipeline);
			remaining = pipeline->child_count;
			while (remaining-- > 0)
				wait(NULL);
			return (1);
		}
		command++;
	}
	close_pipeline(pipeline);
	return (wait_pipeline_bonus(pipeline));
}
