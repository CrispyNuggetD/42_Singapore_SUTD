/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   forks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/10/20 22:46:50 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/* Caller owns both forks; release both even after stopping. */
t_status	release_forks(t_philo *philo)
{
	t_status	status;

	status = STATUS_OK;
	if (pthread_mutex_unlock(philo->second_fork_lock) != 0)
		status = ERR_MUTEX;
	if (pthread_mutex_unlock(philo->first_fork_lock) != 0)
		status = ERR_MUTEX;
	return (status);
}

/*
** Success leaves this fork held. On stopping/error, release it if acquired.
** Caller must not already own this fork; blocking lock is not interruptible.
*/
static t_status	take_fork_check_stop(t_philo *philo, pthread_mutex_t *fork_lock)
{
	t_status	status;

	if (pthread_mutex_lock(fork_lock) != 0)
		return (ERR_MUTEX);
	status = check_or_stop_sim(philo->sim, 0);
	if (status == STATUS_OK)
		status = log_philo_state(philo, LOG_FORK);
	if (status != STATUS_OK && pthread_mutex_unlock(fork_lock) != 0)
		return (ERR_MUTEX);
	return (status);
}

/*
** Success owns both forks. On second-step failure, release the first fork.
** With one fork, wait for stopping/error and release it without relocking.
** Use the fixed acquisition order assigned by init_philos on every meal.
*/
t_status	take_forks(t_philo *philo)
{
	t_status		status;
	pthread_mutex_t	*first;
	pthread_mutex_t	*second;

	first = philo->first_fork_lock;
	second = philo->second_fork_lock;
	status = take_fork_check_stop(philo, first);
	if (status != STATUS_OK)
		return (status);
	if (first == second)
	{
		while (status == STATUS_OK)
			status = philo_wait(philo->sim,
					philo->sim->config.time_to_die);
	}
	else
		status = take_fork_check_stop(philo, second);
	if (status != STATUS_OK && pthread_mutex_unlock(first) != 0)
		return (ERR_MUTEX);
	return (status);
}
