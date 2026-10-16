/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 21:06:49 by hnah              #+#    #+#             */
/*   Updated: 2026/10/12 22:15:28 by hnah             ###   ########.fr       */
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
	while (res->meals_init_count < sim->config.count)
	{
		philo = &res->philos[res->meals_init_count];
		philo->id = res->meals_init_count + 1;
		philo->left_fork_lock = &res->all_forks_mutexes[res->meals_init_count];
		philo->right_fork_lock = &res->all_forks_mutexes[philo->id
			% sim->config.count];
		if (pthread_mutex_init(&philo->meal_state_lock, NULL) != 0)
			return (ERR_MUTEX_INIT);
		philo->last_meal = sim->runtime.start_time;
		philo->meals_eaten = 0;
		philo->sim = sim;
		res->meals_init_count++;
	}
	return (STATUS_OK);
}

static t_status	init_forks(t_sim *sim)
{
	t_resources	*res;

	res = &sim->resources;
	while (res->forks_init_count < sim->config.count)
	{
		if (pthread_mutex_init(&res->all_forks_mutexes[res->forks_init_count],
				NULL) != 0)
			return (ERR_MUTEX_INIT);
		res->forks_init_count++;
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
	// TODO: Track successful initialization of all_stop_lock and printing_lock.
	// WRONG NOW: If printing_lock initialization fails, all_stop_lock remains initialized.
	// Later allocation/setup failures also leave these shared mutexes initialized.
	// Add success flags to t_resources; set each only after its mutex init succeeds.
	sim->runtime.all_stop = 0;
	if (pthread_mutex_init(&sim->runtime.all_stop_lock, NULL) != 0)
		return (ERR_MUTEX_INIT);
	if (pthread_mutex_init(&sim->runtime.printing_lock, NULL) != 0)
		return (ERR_MUTEX_INIT);
	res = &sim->resources;
	res->all_forks_mutexes = malloc(sizeof(*res->all_forks_mutexes)
			* sim->config.count);
	if (res->all_forks_mutexes == NULL)
		return (ERR_MALLOC);
	res->philos = malloc(sizeof(t_philo) * sim->config.count);
	if (res->philos == NULL)
		return (ERR_MALLOC);
	status = init_forks(sim);
	if (status != STATUS_OK)
		return (status);
	return (init_philos(sim));
}
