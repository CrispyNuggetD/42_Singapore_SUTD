/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 22:19:06 by hnah              #+#    #+#             */
/*   Updated: 2026/10/24 22:50:33 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/* Store the opened handle in the caller's semaphore pointer. */
static t_status	open_semaphore(sem_t **sem, const char *name,
		unsigned int value)
{
	*sem = sem_open(name, O_CREAT | O_EXCL, 0600, value);
	if (*sem == SEM_FAILED)
		return (ERR_SEM_OPEN);
	return (STATUS_OK);
}

/* Zero sim before parsing; caller cleans partial initialization on error. */
t_status	init_bonus(t_sim_bonus *sim)
{
	t_status	status;

	status = init_children(&sim->children, sim->config.count);
	if (status != STATUS_OK)
		return (status);
	status = open_semaphore(&sim->forks, SEM_FORKS_NAME,
			sim->config.count);
	if (status != STATUS_OK)
		return (status);
	status = open_semaphore(&sim->take_pair, SEM_PAIR_NAME, 1);
	if (status != STATUS_OK)
		return (status);
	return (open_semaphore(&sim->printing, SEM_PRINT_NAME, 1));
}
