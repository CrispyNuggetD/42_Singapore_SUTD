/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logging.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/10/16 23:20:43 by hnah             ###   ########.fr       */
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
** Log one state supplied by the caller; this function does not detect death.
** The monitor calls LOG_DIED only after confirming an expired deadline.
** Other callers use LOG_FORK, LOG_EATING, LOG_SLEEPING, or LOG_THINKING.
**
** request is an INPUT to check_or_stop_sim, not an operation result:
** ordinary messages pass STATUS_OK (check only); LOG_DIED passes
** STATUS_STOPPED (publish global stopping before printing this death).
** status is the OUTPUT: STATUS_OK permits this message; STATUS_STOPPED
** suppresses it because global stopping was already set. ERR_* is failure.
** The first death request returns STATUS_OK so its message can print.
**
** Lock order is printing_lock then all_stop_lock. The helper releases the
** stop lock before output; this function releases the print lock afterward.
** Local STATUS_STOPPED results from workers do not call this logger as death.
** Return failures to the caller; this function does not record first_error
** for its own time/output errors, join threads, or clean up resources.
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
