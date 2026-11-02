/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logging.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/11/02 17:36:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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
** Log a caller-supplied state; only the monitor supplies confirmed LOG_DIED.
**
** request: STATUS_OK checks stopping; STATUS_STOPPED publishes death stopping.
** status: STATUS_OK permits output; STATUS_STOPPED suppresses it; ERR_* fails.
** The first death request publishes stopping but still permits its own output.
**
** Lock order: printing_lock -> all_stop_lock; release the stop lock before
** output and the printing lock afterward. Return errors for callers to handle.
*/
t_status	log_philo_state(t_philo *philo, t_philo_state state)
{
	t_status	status;
	t_status	request;

	request = STATUS_OK;
	if (state == LOG_DIED)
		request = STATUS_STOPPED;
	if (state < LOG_FORK || state >= LOG_TOTAL)
		return (ERR_INVALID_LOG_STATE);
	if (pthread_mutex_lock(&philo->sim->runtime.printing_lock) != 0)
		return (ERR_MUTEX);
	status = check_or_stop_sim(philo->sim, request);
	if (status == STATUS_OK)
		status = print_message(philo, state);
	if (pthread_mutex_unlock(&philo->sim->runtime.printing_lock) != 0)
		return (ERR_MUTEX);
	return (status);
}
