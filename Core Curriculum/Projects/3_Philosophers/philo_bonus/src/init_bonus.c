/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 22:19:06 by hnah              #+#    #+#             */
/*   Updated: 2026/11/02 17:36:08 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

static t_status	init_philo_monitor_bonus(t_philo_bonus *philo)
{
	if (pthread_create(&philo->monitor_thread, NULL,
			monitor_philo_bonus, philo) != 0)
		return (ERR_THREAD_CREATE);
	return (STATUS_OK);
}

/* Child only: initialize its local meal state and private semaphore handle. */
t_status	init_philo_bonus(t_philo_bonus *philo, t_sim_bonus *sim, int id)
{
	t_status	status;

	memset(philo, 0, sizeof(*philo));
	philo->id = id;
	philo->sim = sim;
	philo->last_meal = sim->start_time;
	status = open_unique_meal_semaphore(philo);
	if (status != STATUS_OK)
		return (status);
	return (init_philo_monitor_bonus(philo));
}

/* Zero sim before parsing; caller cleans partial initialization on error. */
t_status	init_bonus(t_sim_bonus *sim)
{
	t_status	status;

	status = init_children(&sim->children, sim->config.count);
	if (status != STATUS_OK)
		return (status);
	status = open_semaphore(&sim->forks_sem, SEM_FORKS_NAME,
			sim->config.count);
	if (status != STATUS_OK)
		return (status);
	status = open_semaphore(&sim->take_pair_lock, SEM_PAIR_NAME, 1);
	if (status != STATUS_OK)
		return (status);
	return (open_semaphore(&sim->printing_lock, SEM_PRINT_NAME, 1));
}
