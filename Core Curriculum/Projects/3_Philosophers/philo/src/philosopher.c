/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/10/12 23:20:49 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*philo_routine(void *argument)
{
	t_philo		*philo;
	t_status	status;

	philo = argument;
	status = check_sim_running(philo->sim);
	if (status == STATUS_OK && philo->id % 2 == 1)
		status = philo_wait(philo->sim, philo->sim->config.time_to_eat / 2);
	if (status == STATUS_OK)
		status = philo_eat(philo);
	if (status == STATUS_OK)
		status = log_philo_state(philo, LOG_SLEEPING);
	if (status > STATUS_STOPPED)
		print_return_error(status);
	return (NULL);
}

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
