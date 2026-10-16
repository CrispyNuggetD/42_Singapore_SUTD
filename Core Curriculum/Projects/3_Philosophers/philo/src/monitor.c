/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/10/16 23:04:37 by hnah             ###   ########.fr       */
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
** Main's monitoring loop, not a separately created thread.
** Keep meal_state_lock through death detection and the death logging call.
** Release it before returning on either death or an operation failure.
** TODO: Observe worker-requested stopping and read sim_status under its lock.
** Currently worker errors are saved, but this loop does not consume them.
*/
t_status	monitor_sim(t_sim *sim)
{
	t_philo		*check_philo;
	t_status	status;
	int			died;
	int			philo_index;

	while (1)
	{
		philo_index = 0;
		while (philo_index < sim->config.count)
		{
			check_philo = &sim->resources.philos[philo_index];
			if (pthread_mutex_lock(&check_philo->meal_state_lock) != 0)
				return (ERR_MUTEX);
			status = check_death_locked(check_philo, sim, &died);
			if (status == STATUS_OK && died == 1)
				status = log_philo_state(check_philo, LOG_DIED);
			if (pthread_mutex_unlock(&check_philo->meal_state_lock) != 0)
				return (ERR_MUTEX);
			if (status != STATUS_OK || died == 1)
				return (status);
			philo_index++;
		}
		if (usleep(WAIT_CHECK_MICROSECONDS) == -1)
			return (ERR_SLEEP);
	}
}
