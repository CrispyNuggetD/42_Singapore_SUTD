/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 20:55:17 by hnah              #+#    #+#             */
/*   Updated: 2026/11/02 17:36:00 by hnah             ###   ########.fr       */
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

t_status	philo_wait_bonus(long duration_ms)
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

/*
** From mandatory: after the first meal, return 0 or 1 extra slot of time_to_eat.
** Odd IDs wait one extra slot; even IDs do not.
** The last ID of an odd count does not wait an extra slot.
** One philosopher does not wait an extra slot to take their only fork.
*/

static int	needs_extra_wait_bonus(t_philo_bonus *philo)
{
	int	count;
	int	meals_per_cycle;
	int	wait_after_meal;
	int	meal_in_cycle;

	count = philo->sim->config.count;
	if (count < 3 || count % 2 == 0 || philo->meals_eaten < 1)
		return (0);
	meals_per_cycle = count / 2;
	wait_after_meal = (philo->id - 1) / 2 + 1;
	if (philo->id == count)
		wait_after_meal--;
	meal_in_cycle = (philo->meals_eaten - 1) % meals_per_cycle + 1;
	return (meal_in_cycle == wait_after_meal);
}

long	thinking_wait_ms_bonus(t_philo_bonus *philo)
{
	long	rest_target;
	long	extra_wait;

	rest_target = philo->sim->config.time_to_eat;
	if (needs_extra_wait_bonus(philo))
		rest_target *= 2;
	extra_wait = rest_target - philo->sim->config.time_to_sleep;
	if (extra_wait < 0)
		return (0);
	return (extra_wait);
}
