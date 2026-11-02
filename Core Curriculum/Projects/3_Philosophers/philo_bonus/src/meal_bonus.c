/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   meal_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 10:42:44 by hnah              #+#    #+#             */
/*   Updated: 2026/11/02 17:59:49 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/*
** Caller owns both forks; protect the deadline test and meal-start update.
** An expired deadline returns local STATUS_STOPPED and preserves last_meal.
** The child monitor is responsible for detecting and logging death.
** The caller releases both fork units on non-success.
*/
static t_status	record_meal_start_bonus(t_philo_bonus *philo)
{
	long		meal_start;
	t_status	status;

	status = wait_semaphore(philo->meal_state_lock);
	if (status != STATUS_OK)
		return (status);
	meal_start = now_ms();
	if (meal_start == -1)
		status = ERR_TIME;
	else if (meal_start - philo->last_meal >= philo->sim->config.time_to_die)
		status = STATUS_STOPPED;
	else
	{
		status = log_philo_state_bonus(philo, LOG_EATING);
		if (status == STATUS_OK)
			philo->last_meal = meal_start;
	}
	if (sem_post(philo->meal_state_lock) == -1)
		return (ERR_SEM_POST);
	return (status);
}

/*
** Acquire two units from the fork semaphore, then record the meal start.
** Success owns both units; a recording failure attempts to return both.
** Acquisition failures are cleaned up inside take_forks_bonus.
*/
t_status	prepare_to_eat_bonus(t_philo_bonus *philo)
{
	t_status	status;

	status = take_forks_bonus(philo);
	if (status != STATUS_OK)
		return (status);
	status = record_meal_start_bonus(philo);
	if (status != STATUS_OK && release_forks_bonus(philo, 2) != STATUS_OK)
		return (ERR_SEM_POST);
	return (status);
}

t_status	philo_eat_bonus(t_philo_bonus *philo)
{
	t_status	status;

	status = prepare_to_eat_bonus(philo);
	if (status != STATUS_OK)
		return (status);
	status = philo_wait_bonus(philo->sim->config.time_to_eat);
	if (release_forks_bonus(philo, 2) != STATUS_OK)
		return (ERR_SEM_POST);
	if (status != STATUS_OK)
		return (status);
	status = wait_semaphore(philo->meal_state_lock);
	if (status != STATUS_OK)
		return (status);
	philo->meals_eaten++;
	if (philo->sim->config.optional_meal_goal != EAT_FOREVER
		&& philo->meals_eaten >= philo->sim->config.optional_meal_goal)
		status = STATUS_DONE;
	if (sem_post(philo->meal_state_lock) == -1)
		return (ERR_SEM_POST);
	return (status);
}
