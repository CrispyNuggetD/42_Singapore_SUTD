/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 19:59:05 by hnah              #+#    #+#             */
/*   Updated: 2026/10/24 19:01:09 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	now_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
		return (-1);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000);
}

/*
** Shared stop/error bookkeeping only: no death detection or printing here.
** Protect both all_stop and first_error with all_stop_lock.
**
** INPUT request: STATUS_OK reads only; STATUS_STOPPED requests global stop;
** ERR_* records the first genuine error and requests global stop.
** Do not forward a worker's local STATUS_STOPPED here unless a global stop
** is intended: it could suppress the monitor's still-pending death message.
**
** OUTPUT reports the state BEFORE this request: STATUS_OK means previously
** running, including when this call first sets stopping. STATUS_STOPPED means
** already stopped. Mutex failures return ERR_MUTEX. Read first_error separately
** under the lock to obtain the recorded error; this return is not that error.
*/
t_status	check_or_stop_sim(t_sim *sim, t_status request)
{
	int	already_stopped;

	if (pthread_mutex_lock(&sim->runtime.all_stop_lock) != 0)
		return (ERR_MUTEX);
	if (request > STATUS_STOPPED && sim->runtime.first_error == STATUS_OK)
		sim->runtime.first_error = request;
	already_stopped = sim->runtime.all_stop;
	if (!already_stopped && request != STATUS_OK)
		sim->runtime.all_stop = 1;
	if (pthread_mutex_unlock(&sim->runtime.all_stop_lock) != 0)
		return (ERR_MUTEX);
	if (already_stopped)
		return (STATUS_STOPPED);
	return (STATUS_OK);
}

/*
** Return STATUS_OK only when the requested duration has elapsed.
** STATUS_STOPPED means global stopping interrupted the wait; ERR_* is failure.
** No locks/forks are released here; the caller handles what it owns.
*/
t_status	philo_wait(t_sim *sim, long duration_ms)
{
	long		start_time;
	long		current_time;
	t_status	status;

	start_time = now_ms();
	if (start_time == -1)
		return (ERR_TIME);
	while (1)
	{
		status = check_or_stop_sim(sim, 0);
		if (status != STATUS_OK)
			return (status);
		current_time = now_ms();
		if (current_time == -1)
			return (ERR_TIME);
		if (current_time - start_time >= duration_ms)
			return (STATUS_OK);
		if (usleep(WAIT_CHECK_MICROSECONDS) == -1)
			return (ERR_SLEEP);
	}
}

long	thinking_wait_ms(t_philo *philo)
{
	long	rest_target;
	long	extra_wait;

	rest_target = philo->sim->config.time_to_eat;
	if (needs_extra_wait(philo))
		rest_target *= 2;
	extra_wait = rest_target - philo->sim->config.time_to_sleep;
	if (extra_wait < 0)
		return (0);
	return (extra_wait);
}
