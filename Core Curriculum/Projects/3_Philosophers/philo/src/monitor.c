/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/10/12 23:22:15 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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

t_status	monitor_sim(t_sim *sim)
{
	t_philo		*check_philo;
	t_status	status;
	int			died;
	int			i;

	while (1)
	{
		i = 0;
		while (i < sim->config.count)
		{
			check_philo = &sim->resources.philos[i];
			if (pthread_mutex_lock(&check_philo->meal_state_lock) != 0)
				return (ERR_MUTEX);
			status = check_death_locked(check_philo, sim, &died);
			if (status == STATUS_OK && died == 1)
				status = log_philo_state(check_philo, LOG_DIED);
			if (pthread_mutex_unlock(&check_philo->meal_state_lock) != 0)
				return (ERR_MUTEX);
			if (status != STATUS_OK || died == 1)
				return (status);
			i++;
		}
		if (usleep(WAIT_CHECK_MICROSECONDS) == -1)
			return (ERR_SLEEP);
	}
}
