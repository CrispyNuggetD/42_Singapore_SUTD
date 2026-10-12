/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/10/12 18:25:01 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*philo_routine(void *argument)
{
	t_philo	*philo;

	philo = argument;
	if (philo->id % 2 == 0)
		philo_eat(philo);
	printf("Hello from philosopher %d\n", philo->id);
	return (NULL);
}

t_status	philo_wait(t_philo *philo, long duration_ms)
{
	long	start_time;
	long	current_time;
	long	elapsed;

	(void)philo;
	start_time = now_ms();
	if (start_time == -1)
		return (ERR_TIME);
	while (1)
	{
		current_time = now_ms();
		if (current_time == -1)
			return (ERR_TIME);
		elapsed = current_time - start_time;
		if (elapsed >= duration_ms)
			break ;
		if (usleep(WAIT_CHECK_MICROSECONDS) == -1)
			return (ERR_SLEEP);
	}
	return (STATUS_OK);
}

t_status	start_philos(t_sim *sim)
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
	return (STATUS_OK);
}
