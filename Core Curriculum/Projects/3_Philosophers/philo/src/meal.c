/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   meal.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/10/16 21:09:23 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// Must always release owned forks, even after stopping
static t_status	release_forks(t_philo *philo)
{
	t_status	status;

	status = STATUS_OK;
	if (pthread_mutex_unlock(philo->right_fork_lock) != 0)
		status = ERR_MUTEX;
	if (pthread_mutex_unlock(philo->left_fork_lock) != 0)
		status = ERR_MUTEX;
	return (status);
}

/*
** Caller owns both forks; protect the deadline test and meal-start update.
** An expired deadline returns local STATUS_STOPPED and preserves last_meal.
** This does not set all_stop: the monitor must still detect and log death.
** The caller releases both forks on non-success; do not publish this normal
** STATUS_STOPPED as a global stop request from the worker.
*/
static t_status	record_meal_start_time_protected(t_philo *philo)
{
	long		meal_start;
	t_status	status;

	if (pthread_mutex_lock(&philo->meal_state_lock) != 0)
		return (ERR_MUTEX);
	status = STATUS_OK;
	meal_start = now_ms();
	if (meal_start == -1)
		status = ERR_TIME;
	else if (meal_start - philo->last_meal >= philo->sim->config.time_to_die)
		status = STATUS_STOPPED;
	else
	{
		status = log_philo_state(philo, LOG_EATING);
		if (status == STATUS_OK)
			philo->last_meal = meal_start;
	}
	if (pthread_mutex_unlock(&philo->meal_state_lock) != 0)
		return (ERR_MUTEX);
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
** Success leaves both forks held and meal_state_lock released.
** Failure attempts to release every fork acquired here.
** Caller must handle the single-philosopher case before calling this.
*/
t_status	prepare_to_eat(t_philo *philo)
{
	t_status	status;

	status = take_fork_check_stop(philo, philo->left_fork_lock);
	if (status != STATUS_OK)
		return (status);
	status = take_fork_check_stop(philo, philo->right_fork_lock);
	if (status != STATUS_OK)
	{
		if (pthread_mutex_unlock(philo->left_fork_lock) != 0)
			return (ERR_MUTEX);
		return (status);
	}
	status = record_meal_start_time_protected(philo);
	if (status != STATUS_OK && release_forks(philo) != STATUS_OK)
		return (ERR_MUTEX);
	return (status);
}

t_status	philo_eat(t_philo *philo)
{
	t_status	status;

	status = check_or_stop_sim(philo->sim, 0);
	if (status != STATUS_OK)
		return (status);
	status = prepare_to_eat(philo);
	if (status != STATUS_OK)
		return (status);
	status = philo_wait(philo->sim, philo->sim->config.time_to_eat);
	if (release_forks(philo) != STATUS_OK)
		return (ERR_MUTEX);
	if (status != STATUS_OK)
		return (status);
	if (pthread_mutex_lock(&philo->meal_state_lock) != 0)
		return (ERR_MUTEX);
	philo->meals_eaten++;
	if (pthread_mutex_unlock(&philo->meal_state_lock) != 0)
		return (ERR_MUTEX);
	return (STATUS_OK);
}
