/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_handler.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 21:05:36 by hnah              #+#    #+#             */
/*   Updated: 2026/09/17 21:55:50 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static void	child_process(t_pipeline *pipeline, int current_command,
		char **argv, char **envp)
{
	int	output_fd;

	output_fd = pipeline->pipefd[1];
	if (current_command == pipeline->last_command)
		output_fd = pipeline->output_fd;
	if (pipeline->input_fd < 0 || output_fd < 0)
		exit(close_pipeline(pipeline));
	if (dup2(pipeline->input_fd, STDIN_FILENO) < 0
		|| dup2(output_fd, STDOUT_FILENO) < 0)
	{
		perror("dup2");
		exit(close_pipeline(pipeline));
	}
	close_pipeline(pipeline);
	execute_command(argv[current_command], envp);
}

static void	advance_pipe(t_pipeline *pipeline, int current_command)
{
	close_and_void_fd(&pipeline->input_fd);
	if (current_command < pipeline->last_command)
	{
		close_and_void_fd(&pipeline->pipefd[1]);
		pipeline->input_fd = pipeline->pipefd[0];
		pipeline->pipefd[0] = -1;
	}
}

static int	spawn_command(t_pipeline *pipeline, int current_command,
		char **argv, char **envp)
{
	pid_t	pid;

	if (current_command < pipeline->last_command && pipe(pipeline->pipefd) < 0)
		return (-1);
	pid = fork();
	if (pid < 0)
		return (-1);
	if (pid == 0)
		child_process(pipeline, current_command, argv, envp);
	pipeline->child_count++;
	pipeline->last_pid = pid;
	advance_pipe(pipeline, current_command);
	return (0);
}

static int	wait_pipeline(t_pipeline *pipeline)
{
	int	status;
	int	remaining;

	if (waitpid(pipeline->last_pid, &status, 0) < 0)
		return (return_perror("waitpid"));
	remaining = pipeline->child_count - 1;
	while (remaining-- > 0)
		wait(NULL);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

int	run_pipeline(t_pipeline *pipeline, char **argv, char **envp)
{
	int	current_command;
	int	remaining;

	current_command = pipeline->first_command;
	while (current_command <= pipeline->last_command)
	{
		if (spawn_command(pipeline, current_command, argv, envp) < 0)
		{
			perror("pipex");
			close_pipeline(pipeline);
			remaining = pipeline->child_count;
			while (remaining-- > 0)
				wait(NULL);
			return (1);
		}
		current_command++;
	}
	close_pipeline(pipeline);
	return (wait_pipeline(pipeline));
}
