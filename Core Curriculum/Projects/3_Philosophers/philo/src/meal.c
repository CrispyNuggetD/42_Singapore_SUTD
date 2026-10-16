/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   meal.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/10/12 19:37:39 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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
	else
		philo->last_meal = meal_start;
	if (pthread_mutex_unlock(&philo->meal_state_lock) != 0)
		return (ERR_MUTEX);
	return (status);
}

/*
** Success leaves both forks held and meal_state_lock released.
** Failure attempts to release every fork acquired here.
** A sole fork cannot satisfy this helper's two-fork requirement.
** Caller must ensure more than one philosopher..!
*/
t_status	prepare_to_eat(t_philo *philo)
{
	t_status	status;

	if (pthread_mutex_lock(philo->left_fork_lock) != 0)
		return (ERR_MUTEX);
	if (pthread_mutex_lock(philo->right_fork_lock) != 0)
	{
		pthread_mutex_unlock(philo->left_fork_lock);
		return (ERR_MUTEX);
	}
	status = record_meal_start_time_protected(philo);
	if (status != STATUS_OK && release_forks(philo) != STATUS_OK)
		return (ERR_MUTEX);
	return (status);
}

t_status	philo_eat(t_philo *philo)
{
	t_status	status;

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
