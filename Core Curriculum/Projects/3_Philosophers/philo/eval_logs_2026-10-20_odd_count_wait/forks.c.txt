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
*/
t_status	take_forks(t_philo *philo, pthread_mutex_t *first,
		pthread_mutex_t *second)
{
	t_status	status;

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

/*
** Experimental: reverse local pointer order in alternate meal-count batches.
** Human round numbering starts at 1; quotient 1 represents round 2.
** The total does not enforce once-per-round eating or synchronized changes.
** Mixed acquisition orders still need a deadlock-prevention argument.
** Swapping is currently disabled by the experimental if (0) below.
*/
t_status	swap_fork_if_even_round(t_philo *philo,
		pthread_mutex_t **first, pthread_mutex_t **second)
{
	int				even_round;
	pthread_mutex_t	*temp;

	even_round = is_even_round(philo);
	if (even_round == -1)
		return (ERR_MUTEX);
	if (even_round)
	{
		temp = *first;
		*first = *second;
		*second = temp;
	}
	return (STATUS_OK);
}
