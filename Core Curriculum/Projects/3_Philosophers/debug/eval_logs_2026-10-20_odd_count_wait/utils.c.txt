/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/10/20 22:21:20 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** Return 1 for human rounds 2, 4, ...; 0 for rounds 1, 3, ... .
** Return -1 on mutex failure; callers must check this before a Boolean test.
** Requires a positive count and initialized completed_meals_lock.
** This snapshots meal-count batches, not synchronized once-per-philo rounds.
*/
int	is_even_round(t_philo *philo)
{
	int	completed_meals;

	if (pthread_mutex_lock(&philo->sim->runtime.completed_meals_lock) != 0)
		return (-1);
	completed_meals = philo->sim->runtime.completed_meals;
	if (pthread_mutex_unlock(&philo->sim->runtime.completed_meals_lock) != 0)
		return (-1);
	return ((completed_meals / philo->sim->config.count) % 2);
}
