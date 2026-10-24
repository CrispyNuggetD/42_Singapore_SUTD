/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 20:55:17 by hnah              #+#    #+#             */
/*   Updated: 2026/10/24 20:55:17 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/* Semaphore statuses are consecutive, in the same order as this table. */
static const char	*semaphore_error_message(t_status status)
{
	static const char	*messages[] = {
		"Error: Semaphore creation failed.\n",
		"Error: Semaphore close failed.\n",
		"Error: Semaphore unlink failed.\n"
	};

	return (messages[status - ERR_SEM_OPEN]);
}

/* From mandatory: replace Pipex perror with allowed write-based messages. */
static const char	*error_message(t_status status)
{
	static const char	*messages[] = {
		"",
		"",
		("Usage: ./philo_bonus <number_of_philosophers> <time_to_die> "
			"<time_to_eat> <time_to_sleep> [optional_meal_goal]\n"),
		"Error: Negative integers are not allowed.\n",
		"Error: Invalid integer input.\n",
		"Error: Integer input exceeds INT_MAX.\n",
		"Error: Arguments must be greater than zero.\n",
		"Error: Memory allocation failed.\n",
		"Error: Failed to read the current time.\n",
		"Error: Sleep failed.\n",
		"Error: Output write failed.\n",
		"Error: Fork failed.\n",
		"Error: Waiting for a child failed.\n",
		"Error: Stopping a child failed.\n",
		"Bonus scaffold: simulation is not implemented yet.\n"
	};

	return (messages[status]);
}

t_status	print_return_error(t_status status)
{
	const char	*message;
	size_t		length;

	if (status > STATUS_STOPPED && status < ERR_TOTAL)
	{
		if (status >= ERR_SEM_OPEN)
			message = semaphore_error_message(status);
		else
			message = error_message(status);
		length = 0;
		while (message[length])
			length++;
		if (write_guaranteed(STDERR_FILENO, message, length) != STATUS_OK)
			return (ERR_WRITE);
	}
	return (status);
}
