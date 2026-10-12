/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/10/12 16:52:52 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*philo_routine(void *argument)
{
	t_philo	*data;

	data = argument;
	printf("Hello from philosopher %d\n", data->id);
	return (NULL);
}

t_status	start_philos(t_sim *sim)
{
	t_resources	*res;
	t_philo		*philo;

	res = &sim->resources;
	while (res->threads_to_join < sim->config.count)
	{
		philo = &res->philos[res->threads_to_join];
		if (pthread_create(&philo->thread_handle, NULL,
				philo_routine, philo) != 0)
			return (ERR_THREAD_CREATE);
		res->threads_to_join++;
	}
	return (STATUS_OK);
}
