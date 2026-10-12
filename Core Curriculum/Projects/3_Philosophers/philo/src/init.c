/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 21:06:49 by hnah              #+#    #+#             */
/*   Updated: 2026/10/12 17:34:54 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** Caller must allocate philos and initialize fork mutexes first.
** Initialize all philosopher records before starting any threads.
*/
t_status	init_philos(t_sim *sim)
{
	t_resources	*res;
	t_philo		*philo;

	res = &sim->resources;
	while (res->meals_init < sim->config.count)
	{
		philo = &res->philos[res->meals_init];
		philo->id = res->meals_init + 1;
		philo->left_fork = &res->forks[res->meals_init];
		philo->right_fork = &res->forks[(res->meals_init + 1)
			% sim->config.count];
		philo->last_meal = sim->runtime.start_time;
		philo->meals_eaten = 0;
		if (pthread_mutex_init(&philo->meal_mutex, NULL) != 0)
			return (ERR_MUTEX_INIT);
		philo->sim = sim;
		res->meals_init++;
	}
	return (STATUS_OK);
}

static t_status	init_forks(t_sim *sim)
{
	t_resources	*res;

	res = &sim->resources;
	while (res->forks_init < sim->config.count)
	{
		if (pthread_mutex_init(&res->forks[res->forks_init], NULL) != 0)
			return (ERR_MUTEX_INIT);
		res->forks_init++;
	}
	return (STATUS_OK);
}

t_status	init_sim(t_sim *sim)
{
	t_status	status;
	t_resources	*res;

	sim->runtime.start_time = now_ms();
	if (sim->runtime.start_time == -1)
		return (ERR_TIME);
	res = &sim->resources;
	res->forks = malloc(sizeof(pthread_mutex_t) * sim->config.count);
	if (res->forks == NULL)
		return (ERR_MALLOC);
	res->philos = malloc(sizeof(t_philo) * sim->config.count);
	if (res->philos == NULL)
		return (ERR_MALLOC);
	status = init_forks(sim);
	if (status != STATUS_OK)
		return (status);
	return (init_philos(sim));
}
