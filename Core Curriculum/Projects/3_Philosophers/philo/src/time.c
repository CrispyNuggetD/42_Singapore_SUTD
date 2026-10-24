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

/*
** Before the first meal, return 0, 1 or 2 slots of time_to_eat.
** Odd IDs start immediately; even IDs wait one slot.
** The last ID of an odd count waits two slots instead.
** One philosopher starts immediately to take their only fork.
*/
int	initial_wait_slots(t_philo *philo)
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
** After the meal philo just finished, is it their turn to take the
** extra wait? It returns 1 for yes, 0 for no after a completed meal.
** Initial slots: odd IDs start at 0, even IDs at 1, last odd ID at 2.
** Each cycle has count / 2 meals, with one extra slot before the next meal.
** ID pairs share a wait position; the last ID joins the final pair.
** No extra slot for even counts.
**
** wait_after_meal, find after which meal within that pattern that
** philosopher waits extra.
**
** Example, n=9 pattern:
** ID:               1  2 | 3  4 | 5  6 | 7  8 | 9
** wait_after_meal:   1  1 | 2  2 | 3  3 | 4  4 | (4, after decrement)
**
** meal_in_cycle just wraps the pattern with a modulo, i.e.
** it finds where the just-completed meal sits within repeating pattern.
** (it offsets to 1-indexed for convenience).
*/

static int	needs_extra_wait(t_philo *philo)
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
