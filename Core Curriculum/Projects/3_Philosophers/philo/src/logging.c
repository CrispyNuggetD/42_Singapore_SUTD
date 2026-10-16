/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logging.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/10/12 23:22:15 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** Caller holds printing_lock. Return STATUS_STOPPED if output is suppressed.
** An allowed death message also sets stopping before releasing the stop lock.
*/
static t_status	check_message_allowed(t_sim *sim, t_philo_state state)
{
	int	stopped;

	if (pthread_mutex_lock(&sim->runtime.all_stop_lock) != 0)
		return (ERR_MUTEX);
	stopped = sim->runtime.all_stop;
	if (!stopped && state == LOG_DIED)
		sim->runtime.all_stop = 1;
	if (pthread_mutex_unlock(&sim->runtime.all_stop_lock) != 0)
		return (ERR_MUTEX);
	if (stopped)
		return (STATUS_STOPPED);
	return (STATUS_OK);
}

static t_status	print_message(t_philo *philo, t_philo_state state)
{
	static const char	*templates[] = {
		"%ld %d has taken a fork\n",
		"%ld %d is eating\n",
		"%ld %d is sleeping\n",
		"%ld %d is thinking\n",
		"%ld %d died\n"
	};
	long				current_time;

	current_time = now_ms();
	if (current_time == -1)
		return (ERR_TIME);
	if (printf(templates[state],
			current_time - philo->sim->runtime.start_time, philo->id) < 0)
		return (ERR_WRITE);
	return (STATUS_OK);
}

/*
** All state output uses print -> stop lock order.
** Death publishes stopping before printing; later messages are suppressed.
*/
t_status	log_philo_state(t_philo *philo, t_philo_state state)
{
	t_status	status;

	if (state < LOG_FORK || state >= LOG_TOTAL)
		return (ERR_INVALID_LOG_STATE);
	if (pthread_mutex_lock(&philo->sim->runtime.printing_lock) != 0)
		return (ERR_MUTEX);
	status = check_message_allowed(philo->sim, state);
	if (status == STATUS_OK)
		status = print_message(philo, state);
	if (pthread_mutex_unlock(&philo->sim->runtime.printing_lock) != 0)
		return (ERR_MUTEX);
	return (status);
}
