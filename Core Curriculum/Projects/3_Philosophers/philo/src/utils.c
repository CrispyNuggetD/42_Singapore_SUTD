/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/10/24 17:10:05 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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

int	needs_extra_wait(t_philo *philo)
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
