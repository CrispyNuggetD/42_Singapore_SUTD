/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 20:55:17 by hnah              #+#    #+#             */
/*   Updated: 2026/10/24 20:55:17 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/*
** New bonus behavior: kill only positive, unreaped PIDs owned by the parent.
** SIGKILL also stops a child blocked in sem_wait; no shared stop flag needed.
** Terminating a process does NOT restore semaphore tokens it acquired.
** The whole simulation must stop; close/unlink semaphore resources afterward.
*/
static t_status	kill_children(t_children *children)
{
	int			i;
	t_status	status;

	i = 0;
	status = STATUS_OK;
	while (i < children->count)
	{
		if (children->pids[i] > 0)
		{
			if (kill(children->pids[i], SIGKILL) < 0 && errno != ESRCH)
				status = ERR_KILL;
		}
		i++;
	}
	return (status);
}

/* From pipex */
/* Adapt failure cleanup: waitpid replaces wait; kill before waiting. */
t_status	cleanup_children(t_children *children)
{
	t_status	status;
	int			exit_code;

	status = kill_children(children);
	if (status != STATUS_OK)
		return (status);
	while (children->alive > 0)
	{
		status = wait_next_child(children, &exit_code);
		if (status != STATUS_OK)
			return (status);
	}
	free(children->pids);
	memset(children, 0, sizeof(*children));
	return (STATUS_OK);
}

/*
** Parent only: a valid handle came from our successful O_CREAT | O_EXCL.
** NULL or SEM_FAILED owns neither a handle nor a name; skip both operations.
** Keep the handle and name on close failure. Report a failed unlink separately.
*/
static t_status	close_semaphore(sem_t **sem, const char *name)
{
	if (*sem == NULL || *sem == SEM_FAILED)
		return (STATUS_OK);
	if (sem_close(*sem) == -1)
		return (ERR_SEM_CLOSE);
	*sem = NULL;
	if (sem_unlink(name) == -1 && errno != ENOENT)
		return (ERR_SEM_UNLINK);
	return (STATUS_OK);
}

/*
** Parent only: stop/reap children before releasing semaphores, in reverse order.
** Keep the original error; otherwise return the first semaphore cleanup error.
** Try all semaphore cleanup even if one fails. Main prints the final status.
*/
t_status	cleanup_bonus(t_sim_bonus *sim, t_status status)
{
	t_status	cleanup_status;

	cleanup_status = cleanup_children(&sim->children);
	if (cleanup_status != STATUS_OK)
		return (cleanup_status);
	cleanup_status = close_semaphore(&sim->printing, SEM_PRINT_NAME);
	if (status <= STATUS_STOPPED && cleanup_status != STATUS_OK)
		status = cleanup_status;
	cleanup_status = close_semaphore(&sim->take_pair, SEM_PAIR_NAME);
	if (status <= STATUS_STOPPED && cleanup_status != STATUS_OK)
		status = cleanup_status;
	cleanup_status = close_semaphore(&sim->forks, SEM_FORKS_NAME);
	if (status <= STATUS_STOPPED && cleanup_status != STATUS_OK)
		status = cleanup_status;
	return (status);
}
