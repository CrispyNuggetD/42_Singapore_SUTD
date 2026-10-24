/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 22:59:37 by hnah              #+#    #+#             */
/*   Updated: 2026/10/24 22:59:37 by hnah             ###   ########.fr       */
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
	(void)sim;
	(void)id;
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
