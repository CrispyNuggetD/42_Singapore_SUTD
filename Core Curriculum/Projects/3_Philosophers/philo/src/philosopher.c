/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/10/24 14:05:45 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/* Stop or failure skips the remaining sleep/think steps. */
static t_status	sleep_and_think(t_philo *philo)
{
	t_status	status;

	status = log_philo_state(philo, LOG_SLEEPING);
	if (status != STATUS_OK)
		return (status);
	status = philo_wait(philo->sim, philo->sim->config.time_to_sleep);
	if (status != STATUS_OK)
		return (status);
	return (log_philo_state(philo, LOG_THINKING));
}

/*
** The initial stagger runs once; the cycle then repeats while status is OK.
** STATUS_STOPPED ends this routine without publishing a new global stop.
** Only ERR_* is forwarded to record the first error and request global stop.
** Meal helpers release owned forks before returning a non-success status.
** This worker leaves error printing to main after the workers are joined.
*/
void	*philo_routine(void *argument)
{
	t_philo		*philo;
	t_status	status;


	philo = argument;
	status = check_or_stop_sim(philo->sim, STATUS_OK);
	while (status == STATUS_OK)
	{
		if (philo->id % 2 == 1 && (philo->sim->config.count % 2 == 1
			&& !is_even_round(philo)))
		status = philo_wait(philo->sim,
				philo->sim->config.time_to_eat / 2);
		status = philo_eat(philo);
		if (status == STATUS_OK)
			status = sleep_and_think(philo);
		if (status != STATUS_OK)
			break ;
		status = check_or_stop_sim(philo->sim, 0);
	}
	if (status > STATUS_STOPPED)
		check_or_stop_sim(philo->sim, status);
	return (NULL);
}

/*
** Start workers, then monitor in main. This function does not join workers.
** TODO: Request stopping on startup/monitor failure before cleanup joins.
** TODO: Main must consume the first shared first_error after joining workers.
*/
t_status	start_sim(t_sim *sim)
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
	return (monitor_sim(sim));
}

/*
** Proposed schedule only: caller supplies cur_phase, starting at zero.
** This helper does not choose or advance that phase.
** Requires count >= 2 and cur_phase >= 0; handle one philosopher separately.
**
** position_index: convert the philosopher's 1-based ID to a 0-based seat.
** phase_start_position: wrap the phase to the seat where this phase starts.
** philo_offset: subtract that start to find the relative seat, then wrap.
** Add count to avoid a negative remainder, then wrap back into the table.
**
** Odd offsets become eligible after one phase advances the starting seat.
** For odd counts, the final seat neighbours the first and must wait twice.
** Other even offsets may eat now. Return a phase distance, not milliseconds.
*/
static int	phases_until_eating(t_philo *philo, int cur_phase)
{
	int	count;
	int	position_index;
	int	phase_start_position;
	int	philo_offset;

	count = philo->sim->config.count;
	position_index = philo->id - 1;
	phase_start_position = cur_phase % count;
	philo_offset = position_index - phase_start_position;
	philo_offset = (philo_offset + count) % count;
	if (philo_offset % 2 != 0)
		return (1);
	if (count % 2 != 0 && philo_offset == count - 1)
		return (2);
	return (0);
}
