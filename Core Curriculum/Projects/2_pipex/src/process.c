/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 00:00:00 by hnah              #+#    #+#             */
/*   Updated: 2026/08/27 00:00:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static void	child_one(t_fds *fds, char **argv, char **envp)
{
	if (fds->infile < 0)
	{
		fds_close(fds);
		exit(1);
	}
	if (dup2(fds->infile, STDIN_FILENO) < 0)
		exit_perror("dup2", 1);
	if (dup2(fds->pipefd[1], STDOUT_FILENO) < 0)
		exit_perror("dup2", 1);
	fds_close(fds);
	execute_command(argv[2], envp);
}

static void	child_two(t_fds *fds, char **argv, char **envp)
{
	if (dup2(fds->pipefd[0], STDIN_FILENO) < 0)
		exit_perror("dup2", 1);
	if (dup2(fds->outfile, STDOUT_FILENO) < 0)
		exit_perror("dup2", 1);
	fds_close(fds);
	execute_command(argv[3], envp);
}

static int	wait_children(pid_t first, pid_t second)
{
	int	status;

	waitpid(first, NULL, 0);
	if (waitpid(second, &status, 0) < 0)
		return (perror("waitpid"), 1);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

int	spawn_children(t_fds *fds, char **argv, char **envp)
{
	pid_t	first;
	pid_t	second;

	first = fork();
	if (first < 0)
		return (perror("fork"), 1);
	if (first == 0)
		child_one(fds, argv, envp);
	second = fork();
	if (second < 0)
		return (fds_close(fds), waitpid(first, NULL, 0), perror("fork"), 1);
	if (second == 0)
		child_two(fds, argv, envp);
	fds_close(fds);
	return (wait_children(first, second));
}
