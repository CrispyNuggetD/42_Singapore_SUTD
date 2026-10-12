/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 19:59:05 by hnah              #+#    #+#             */
/*   Updated: 2026/10/12 23:11:29 by hnah             ###   ########.fr       */
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
** Copy the shared stop flag while locked; return only after unlocking.
** STATUS_STOPPED is a normal interrupted wait, not a completed duration.
*/
t_status	check_sim_running(t_sim *sim)
{
	int	stop;

	if (pthread_mutex_lock(&sim->runtime.pls_stop.lock) != 0)
		return (ERR_MUTEX);
	stop = sim->runtime.pls_stop.value;
	if (pthread_mutex_unlock(&sim->runtime.pls_stop.lock) != 0)
		return (ERR_MUTEX);
	if (stop)
		return (STATUS_STOPPED);
	return (STATUS_OK);
}

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
		status = check_sim_running(sim);
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
