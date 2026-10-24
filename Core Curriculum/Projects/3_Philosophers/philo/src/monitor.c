/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/10/20 17:01:06 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** Caller holds meal_state_lock. This only tests this philosopher's deadline.
** STATUS_OK means the check worked; *died separately reports alive/dead.
** Do not set global stopping here: LOG_DIED publishes it and prints once.
*/
static t_status	check_death_locked(t_philo *philo, t_sim *sim, int *died)
{
	long		current_time;
	long		time_since_last_meal;

	*died = 0;
	current_time = now_ms();
	if (current_time == -1)
		return (ERR_TIME);
	time_since_last_meal = current_time - philo->last_meal;
	if (time_since_last_meal >= sim->config.time_to_die)
		*died = 1;
	return (STATUS_OK);
}

/*
** Check death and meal completion under one meal-state lock.
** Keep the lock through death logging; unlock before returning any result.
** A successfully announced death returns STATUS_STOPPED; preserve errors.
*/
static t_status	check_philo(t_philo *philo, int *eat_done)
{
	t_status	status;
	int			died;

	if (pthread_mutex_lock(&philo->meal_state_lock) != 0)
		return (ERR_MUTEX);
	status = check_death_locked(philo, philo->sim, &died);
	if (philo->sim->config.optional_meal_goal != EAT_FOREVER
		&& philo->meals_eaten >= philo->sim->config.optional_meal_goal)
		(*eat_done)++;
	if (status == STATUS_OK && died == 1)
		status = log_philo_state(philo, LOG_DIED);
	if (pthread_mutex_unlock(&philo->meal_state_lock) != 0)
		return (ERR_MUTEX);
	if (status == STATUS_OK && died == 1)
		return (STATUS_STOPPED);
	return (status);
}

/*
** Main's monitoring loop, not a separately created thread.
** Scan once for both death and the meal goal; stop normally when all finish.
** Observe worker-requested stopping before checking each philosopher.
** Cleanup reads the first shared error after joining all started workers.
*/
t_status	monitor_sim(t_sim *sim)
{
	t_status	status;
	int			philo_index;
	int			eat_done;

	while (1)
	{
		philo_index = 0;
		eat_done = 0;
		while (philo_index < sim->config.count)
		{
			status = check_or_stop_sim(sim, STATUS_OK);
			if (status != STATUS_OK)
				return (status);
			status = check_philo(&sim->resources.philos[philo_index],
					&eat_done);
			if (status != STATUS_OK)
				return (status);
			philo_index++;
		}
		if (sim->config.optional_meal_goal != EAT_FOREVER
			&& eat_done == sim->config.count)
			return (check_or_stop_sim(sim, STATUS_STOPPED));
		if (usleep(WAIT_CHECK_MICROSECONDS) == -1)
			return (ERR_SLEEP);
	}
}
