/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 20:55:17 by hnah              #+#    #+#             */
/*   Updated: 2026/10/24 20:55:17 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/* From pipex */
/* Adapt init_pipeline: track all child PIDs, not only the last command. */
t_status	init_children(t_children *children, int count)
{
	memset(children, 0, sizeof(*children));
	children->pids = malloc(sizeof(*children->pids) * count);
	if (children->pids == NULL)
		return (ERR_MALLOC);
	memset(children->pids, 0, sizeof(*children->pids) * count);
	children->count = count;
	return (STATUS_OK);
}

/* From pipex */
/*
** Adapt spawn_command: no pipe, dup2 or execve; caller supplies the routine.
** Call once per unused index, from 0 to count - 1, after init_children.
** Both processes return OK: *pid == 0 identifies the child branch.
** Child MUST run its routine and exit; only the parent continues spawning.
*/
t_status	fork_philo(t_children *children, int index, pid_t *pid)
{
	*pid = fork();
	if (*pid < 0)
		return (ERR_FORK);
	if (*pid > 0)
	{
		children->pids[index] = *pid;
		children->alive++;
	}
	return (STATUS_OK);
}

/* From pipex */
/* wait_pipeline's status decoding; called only after waitpid succeeds. */
static int	child_exit_code(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (CHILD_ERROR);
}

static void	forget_child(t_children *children, pid_t pid)
{
	int	i;

	i = 0;
	while (i < children->count)
	{
		if (children->pids[i] == pid)
		{
			children->pids[i] = 0;
			children->alive--;
			return ;
		}
		i++;
	}
}

/* From pipex */
/*
** Adapt wait_pipeline: wait for ANY child, not the last-created philosopher.
** Parent only, alive > 0; own only the children tracked here. Retry EINTR.
** Return is wait success/error; *exit_code is the separate child outcome.
** TODO: parent reacts to CHILD_DIED/CHILD_ERROR by stopping other children.
*/
t_status	wait_next_child(t_children *children, int *exit_code)
{
	pid_t	pid;
	int		status;

	pid = waitpid(-1, &status, 0);
	while (pid < 0 && errno == EINTR)
		pid = waitpid(-1, &status, 0);
	if (pid < 0)
		return (ERR_WAITPID);
	forget_child(children, pid);
	*exit_code = child_exit_code(status);
	return (STATUS_OK);
}
