/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/10/16 23:05:53 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** The initial stagger runs once; the cycle then repeats while status is OK.
** STATUS_STOPPED ends this routine without publishing a new global stop.
** Only ERR_* is forwarded to record the first error and request global stop.
** Meal helpers release owned forks before returning a non-success status.
** This worker leaves error printing to main after the workers are joined.
*/
void	*philo_routine(void *argument)
{
	t_philo		*philo;
	t_status	status;

	philo = argument;
	if (philo->id % 2 == 1)
	{
		status = philo_wait(philo->sim, philo->sim->config.time_to_eat / 2);
		if (status > STATUS_STOPPED)
		{
			check_or_stop_sim(philo->sim, status);
			return (NULL);
		}
	}
	status = check_or_stop_sim(philo->sim, 0);
	while (status == STATUS_OK)
	{
		status = philo_eat(philo);
		if (status == STATUS_OK)
			status = log_philo_state(philo, LOG_SLEEPING);
		if (status == STATUS_OK)
			status = philo_wait(philo->sim, philo->sim->config.time_to_sleep);
		if (status == STATUS_OK)
			status = log_philo_state(philo, LOG_THINKING);
		if (status != STATUS_OK)
			break ;
		status = check_or_stop_sim(philo->sim, 0);
	}
	if (status > STATUS_STOPPED)
		check_or_stop_sim(philo->sim, status);
	return (NULL);
}

/*
** Start workers, then monitor in main. This function does not join workers.
** TODO: Request stopping on startup/monitor failure before cleanup joins.
** TODO: Main must consume the first shared sim_status after joining workers.
*/
t_status	start_sim(t_sim *sim)
{
	t_resources	*res;
	t_philo		*next_philo;

	res = &sim->resources;
	while (res->threads_to_join < sim->config.count)
	{
		next_philo = &res->philos[res->threads_to_join];
		if (pthread_create(&next_philo->thread_handle, NULL,
				philo_routine, next_philo) != 0)
			return (ERR_THREAD_CREATE);
		res->threads_to_join++;
	}
	return (monitor_sim(sim));
}
