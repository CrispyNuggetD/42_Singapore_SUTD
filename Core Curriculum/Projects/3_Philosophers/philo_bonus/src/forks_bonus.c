/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   forks_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/11/02 11:10:43 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/* Attempt every release, even if an earlier post fails. */
t_status	release_forks_bonus(t_philo_bonus *philo, int count)
{
	t_status	status;

	status = STATUS_OK;
	while (count > 0)
	{
		if (sem_post(philo->sim->forks) == -1)
			status = ERR_SEM_POST;
		count--;
	}
	return (status);
}

/* Signals may interrupt a wait without acquiring a semaphore unit. */
static t_status	wait_semaphore(sem_t *sem)
{
	while (sem_wait(sem) == -1)
	{
		if (errno != EINTR)
			return (ERR_SEM_WAIT);
	}
	return (STATUS_OK);
}

/* Success owns one unit; logging failure returns that unit. */
static t_status	take_fork_and_log(t_philo_bonus *philo)
{
	t_status	status;

	status = wait_semaphore(philo->sim->forks);
	if (status != STATUS_OK)
		return (status);
	status = log_philo_state_bonus(philo, LOG_FORK);
	if (status != STATUS_OK && release_forks_bonus(philo, 1) != STATUS_OK)
		return (ERR_SEM_POST);
	return (status);
}

/*
** Serialize pair acquisition, then release the gate before eating.
** Failure attempts to return all owned units and the acquisition gate.
** One philosopher blocks on the second unit until its monitor ends the child.
*/
t_status	take_forks_bonus(t_philo_bonus *philo)
{
	t_status	status;
	int			count;

	status = wait_semaphore(philo->sim->take_pair);
	if (status != STATUS_OK)
		return (status);
	count = 0;
	while (count < 2 && status == STATUS_OK)
	{
		status = take_fork_and_log(philo);
		if (status == STATUS_OK)
			count++;
	}
	if (sem_post(philo->sim->take_pair) == -1)
		status = ERR_SEM_POST;
	if (status != STATUS_OK && release_forks_bonus(philo, count) != STATUS_OK)
		return (ERR_SEM_POST);
	return (status);
}
