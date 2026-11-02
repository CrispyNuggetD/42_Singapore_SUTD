/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 22:59:37 by hnah              #+#    #+#             */
/*   Updated: 2026/11/02 18:39:15 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/* Stop or failure skips the remaining sleep/think steps. */
static t_status	sleep_and_think_bonus(t_philo_bonus *philo)
{
	t_status	status;

	status = log_philo_state_bonus(philo, LOG_SLEEPING);
	if (status != STATUS_OK)
		return (status);
	status = philo_wait_bonus(philo->sim->config.time_to_sleep);
	if (status != STATUS_OK)
		return (status);
	return (log_philo_state_bonus(philo, LOG_THINKING));
}

/* Join the monitor before cleaning this child after quota completion. */
static int	finish_philo_bonus(t_philo_bonus *philo, t_status status)
{
	if (status != STATUS_STOPPED && status != STATUS_DONE)
		exit(CHILD_ERROR);
	if (pthread_join(philo->monitor_thread, NULL) != 0)
		exit(CHILD_ERROR);
	if (status == STATUS_DONE)
	{
		status = cleanup_philo_bonus(philo, philo->sim);
		if (status == STATUS_OK)
			exit(CHILD_DONE);
	}
	exit(CHILD_ERROR);
}

/*
** Initialize this child and its monitor, then run the philosopher cycle.
** Initialization failure returns CHILD_ERROR before a monitor is running.
** Later errors exit the child; an expired routine waits for its monitor.
** Parent cleanup owns sibling termination and shared semaphore names.
*/
int	run_philo_bonus(t_sim_bonus *sim, int id)
{
	t_philo_bonus	philo;
	t_status		status;

	status = init_philo_bonus(&philo, sim, id);
	if (status != STATUS_OK)
		return (CHILD_ERROR);
	status = philo_wait_bonus(initial_wait_slots_bonus(&philo)
			* sim->config.time_to_eat);
	while (status == STATUS_OK)
	{
		status = philo_eat_bonus(&philo);
		if (status == STATUS_OK)
			status = sleep_and_think_bonus(&philo);
		if (status == STATUS_OK)
			status = philo_wait_bonus(thinking_wait_ms_bonus(&philo));
		if (status != STATUS_OK)
			break ;
	}
	return (finish_philo_bonus(&philo, status));
}

/* Parent creates all children; each child runs one routine and then exits. */
t_status	start_bonus(t_sim_bonus *sim)
{
	t_status	status;
	pid_t		pid;
	int			i;

	i = 0;
	sim->start_time = now_ms();
	if (sim->start_time == -1)
		return (ERR_TIME);
	while (i < sim->config.count)
	{
		status = fork_philo_bonus(&sim->children, i, &pid);
		if (status != STATUS_OK)
			return (status);
		if (pid == 0)
			exit(run_philo_bonus(sim, i + 1));
		i++;
	}
	return (supervise_children_bonus(sim));
}
