/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/10/24 19:01:28 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/* Stop or failure skips the remaining sleep/think steps. */
static t_status	sleep_and_think(t_philo *philo)
{
	t_status	status;

	status = log_philo_state(philo, LOG_SLEEPING);
	if (status != STATUS_OK)
		return (status);
	status = philo_wait(philo->sim, philo->sim->config.time_to_sleep);
	if (status != STATUS_OK)
		return (status);
	return (log_philo_state(philo, LOG_THINKING));
}

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
	status = check_or_stop_sim(philo->sim, STATUS_OK);
	if (status == STATUS_OK)
		status = philo_wait(philo->sim, initial_wait_slots(philo)
				* philo->sim->config.time_to_eat);
	while (status == STATUS_OK)
	{
		status = philo_eat(philo);
		if (status == STATUS_OK)
			status = sleep_and_think(philo);
		if (status == STATUS_OK)
			status = philo_wait(philo->sim, thinking_wait_ms(philo));
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
** Main requests stopping on startup/monitor failure before cleanup joins.
** Cleanup reads the first shared error after joining all started workers.
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
