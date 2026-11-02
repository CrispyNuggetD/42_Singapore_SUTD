/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logging_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 15:25:40 by hnah              #+#    #+#             */
/*   Updated: 2026/11/02 17:36:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/* Convert a signed value without overflowing when negating LONG_MIN. */
static void	append_number(char *buffer, long value, size_t *length)
{
	char			digits[sizeof(long) * 3];
	unsigned long	number;
	int				count;

	number = (unsigned long)value;
	if (value < 0)
	{
		buffer[(*length)++] = '-';
		number = 0UL - number;
	}
	count = 0;
	while (number >= 10)
	{
		digits[count++] = '0' + number % 10;
		number /= 10;
	}
	buffer[(*length)++] = '0' + number;
	while (count > 0)
		buffer[(*length)++] = digits[--count];
}

static size_t	format_message(char *buffer, long elapsed, int id,
		const char *message)
{
	size_t	length;

	length = 0;
	append_number(buffer, elapsed, &length);
	buffer[length++] = ' ';
	append_number(buffer, id, &length);
	buffer[length++] = ' ';
	while (*message)
		buffer[length++] = *message++;
	return (length);
}

/* Buffer fits the timestamp, ID, separators and longest message. */
static t_status	print_message(t_philo_bonus *philo, t_philo_state state)
{
	static const char	*messages[] = {
		"has taken a fork\n",
		"is eating\n",
		"is sleeping\n",
		"is thinking\n",
		"died\n"
	};
	char				buffer[sizeof(long) * 3 + 64];
	long				current_time;
	size_t				length;

	current_time = now_ms();
	if (current_time == -1)
		return (ERR_TIME);
	length = format_message(buffer, current_time - philo->sim->start_time,
			philo->id, messages[state]);
	return (write_guaranteed(STDOUT_FILENO, buffer, length));
}

/*
** Serialize caller-supplied messages using the shared printing semaphore.
** Release it after ordinary messages or print failures; propagate errors.
** Keep it held after a successful LOG_DIED to block further messages.
** The death caller must then exit the child so the parent can stop its peers.
** This function does not detect death or stop processes itself.
*/
t_status	log_philo_state_bonus(t_philo_bonus *philo, t_philo_state state)
{
	t_status	status;

	if (state < LOG_FORK || state >= LOG_TOTAL)
		return (ERR_INVALID_LOG_STATE);
	status = wait_semaphore(philo->sim->printing_lock);
	if (status != STATUS_OK)
		return (status);
	status = print_message(philo, state);
	if ((state != LOG_DIED || status != STATUS_OK)
		&& sem_post(philo->sim->printing_lock) == -1)
		return (ERR_SEM_POST);
	return (status);
}
