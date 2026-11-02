/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 20:55:17 by hnah              #+#    #+#             */
/*   Updated: 2026/11/02 10:37:57 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/* From mandatory: same gettimeofday-to-milliseconds conversion. */
long	now_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
		return (-1);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000);
}

/*
** From mandatory: elapsed-time wait, without the thread-shared stop flag.
** This only waits or reports errors. It does not monitor death or stop peers.
** The bonus child monitor and parent shutdown still need to be implemented.
*/
t_status	wait_ms(long duration_ms)
{
	long	start_time;
	long	current_time;

	start_time = now_ms();
	if (start_time == -1)
		return (ERR_TIME);
	while (1)
	{
		current_time = now_ms();
		if (current_time == -1)
			return (ERR_TIME);
		if (current_time - start_time >= duration_ms)
			return (STATUS_OK);
		if (usleep(WAIT_CHECK_MICROSECONDS) == -1)
			return (ERR_SLEEP);
	}
}

/*
** Before the first meal, return 0, 1 or 2 slots of time_to_eat.
** Odd IDs start immediately; even IDs wait one slot.
** The last ID of an odd count waits two slots instead.
** One philosopher starts immediately to take their only fork.
*/
int	initial_wait_slots_bonus(t_philo_bonus *philo)
{
	int	count;

	count = philo->sim->config.count;
	if (count == 1)
		return (0);
	if (count % 2 == 1 && philo->id == count)
		return (2);
	if (philo->id % 2 == 0)
		return (1);
	return (0);
}

t_status	philo_wait_bonus(t_sim_bonus *sim, long duration_ms)
{
	long		start_time;
	long		current_time;

	start_time = now_ms();
	if (start_time == -1)
		return (ERR_TIME);
	while (1)
	{
		current_time = now_ms();
		if (current_time == -1)
			return (ERR_TIME);
		if (current_time - start_time >= duration_ms)
			return (STATUS_OK);
		if (usleep(WAIT_CHECK_MICROSECONDS) == -1)
			return (ERR_SLEEP);
	}
}
