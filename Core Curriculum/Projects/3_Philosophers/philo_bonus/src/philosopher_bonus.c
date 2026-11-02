/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 22:59:37 by hnah              #+#    #+#             */
/*   Updated: 2026/11/02 10:55:41 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/*
** TODO: child meal state, fork acquisition, routine and death-monitor thread.
** Return a CHILD_* exit code, not a t_status. Parent resources are not cleaned
** here: the child must not kill siblings or unlink the parent's semaphore names.
** This stub is not called by main while its not-implemented guard remains.
*/
int	run_philo_bonus(t_sim_bonus *sim, int id)
{
	t_philo_bonus	philo;
	t_status		status;

	memset(&philo, 0, sizeof(philo));
	philo.id = id;
	philo.sim = sim;
	philo.last_meal = sim->start_time;
	/* Open this philosopher's meal_state_lock before starting its monitor. */

	status = philo_wait_bonus(sim,
		initial_wait_slots_bonus(&philo) * sim->config.time_to_eat);
	while (status == STATUS_OK)
	{
		status = philo_eat_bonus(&philo);
		if (status == STATUS_OK)
			status = sleep_and_think(sim);
		if (status == STATUS_OK)
			status = philo_wait_bonus(sim, thinking_wait_ms(sim));
		if (status != STATUS_OK)
			break ;
	}
	return (CHILD_ERROR);
}

/* Parent creates all children; each child runs one routine and then exits. */
t_status	start_bonus(t_sim_bonus *sim)
{
	t_status	status;
	pid_t		pid;
	int			i;

	i = 0;
	while (i < sim->config.count)
	{
		status = fork_philo(&sim->children, i, &pid);
		if (status != STATUS_OK)
			return (status);
		if (pid == 0)
			exit(run_philo_bonus(sim, i + 1));
		i++;
	}
	return (supervise_children(sim));
}
