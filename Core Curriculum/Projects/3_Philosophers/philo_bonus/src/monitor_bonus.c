/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 22:59:37 by hnah              #+#    #+#             */
/*   Updated: 2026/11/02 18:38:19 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/*
** Parent waits for child outcomes, not direct hunger checks.
** CHILD_DONE: keep waiting. CHILD_DIED: return STATUS_STOPPED for main cleanup.
** Child/wait failures: return an ERR_* so main stops and reaps the others.
** Hunger detection belongs to a monitoring thread inside each child process.
*/
t_status	supervise_children_bonus(t_sim_bonus *sim)
{
	t_status	status;
	int			exit_code;

	exit_code = 0;
	while (sim->children.alive_to_reap > 0)
	{
		status = wait_next_child(&sim->children, &exit_code);
		if (status != STATUS_OK)
			return (status);
		if (exit_code == CHILD_DIED)
			return (STATUS_STOPPED);
		if (exit_code != CHILD_DONE)
			return (ERR_CHILD);
	}
	return (STATUS_OK);
}

static t_status	check_death_locked_bonus(t_philo_bonus *philo, int *died)
{
	long		current_time;
	long		time_since_last_meal;

	*died = 0;
	current_time = now_ms();
	if (current_time == -1)
		return (ERR_TIME);
	time_since_last_meal = current_time - philo->last_meal;
	if (time_since_last_meal >= philo->sim->config.time_to_die)
		*died = 1;
	return (STATUS_OK);
}

/* Caller holds the meal lock; terminal outcomes end this child process. */
static void	check_philo_locked_bonus(t_philo_bonus *philo)
{
	t_status	status;
	int			died;

	status = check_death_locked_bonus(philo, &died);
	if (status != STATUS_OK)
		exit(CHILD_ERROR);
	if (died)
	{
		status = log_philo_state_bonus(philo, LOG_DIED);
		if (status != STATUS_OK)
			exit(CHILD_ERROR);
		exit(CHILD_DIED);
	}
}

void	*monitor_philo_bonus(void *argument)
{
	t_philo_bonus	*philo;
	t_status		status;

	status = STATUS_OK;
	philo = argument;
	while (1)
	{
		if (wait_semaphore(philo->meal_state_lock) != STATUS_OK)
			exit(CHILD_ERROR);
		if (philo->sim->config.optional_meal_goal != EAT_FOREVER
			&& philo->meals_eaten >= philo->sim->config.optional_meal_goal)
			status = STATUS_DONE;
		else
			check_philo_locked_bonus(philo);
		if (sem_post(philo->meal_state_lock) == -1)
			exit(CHILD_ERROR);
		if (status == STATUS_DONE)
			return (NULL);
		if (usleep(WAIT_CHECK_MICROSECONDS) == -1)
			exit(CHILD_ERROR);
	}
}
