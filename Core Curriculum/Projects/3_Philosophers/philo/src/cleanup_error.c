/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_error.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 21:06:49 by hnah              #+#    #+#             */
/*   Updated: 2026/10/12 17:25:39 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

t_status	join_philos(t_resources *res)
{
	while (res->threads_to_join > 0)
	{
		if (pthread_join(res->philos[res->threads_to_join - 1].thread_handle,
				NULL) != 0)
			return (ERR_THREAD_JOIN);
		res->threads_to_join--;
	}
	return (STATUS_OK);
}

/*
** Join started threads before destroying mutexes or freeing their data.
** Retain arrays if joining a thread or destroying a mutex fails.
*/
int	cleanup_resources(t_resources *res, t_status status)
{
	if (join_philos(res) != STATUS_OK)
		return (print_return_error(ERR_THREAD_JOIN) != STATUS_OK);
	while (res->meals_init > 0)
	{
		if (pthread_mutex_destroy(
				&res->philos[res->meals_init - 1].meal_mutex) != 0)
			return (print_return_error(ERR_MUTEX_DESTROY) != STATUS_OK);
		res->meals_init--;
	}
	while (res->forks_init > 0)
	{
		if (pthread_mutex_destroy(&res->forks[res->forks_init - 1]) != 0)
			return (print_return_error(ERR_MUTEX_DESTROY) != STATUS_OK);
		res->forks_init--;
	}
	free(res->philos);
	free(res->forks);
	res->philos = NULL;
	res->forks = NULL;
	return (print_return_error(status) > STATUS_STOPPED);
}

/*
** printf writes to stdout, so I use write to send errors to stderr here.
** This function reports the error and returns its status.
*/
static const char	*error_message(t_status status)
{
	static const char	*messages[] = {
		"",
		"",
		("Usage: ./philo <number_of_philosophers> <time_to_die> "
			"<time_to_eat> <time_to_sleep> [optional_meal_goal]\n"),
		"Error: Negative integers are not allowed.\n",
		"Error: Invalid integer input.\n",
		"Error: Integer input exceeds INT_MAX.\n",
		"Error: Arguments must be greater than zero.\n",
		"Error: Memory allocation failed.\n",
		"Error: Mutex initialization failed.\n",
		"Error: Failed to read the current time.\n",
		"Error: Thread creation failed.\n",
		"Error: Thread join failed.\n",
		"Error: Mutex destruction failed.\n",
		"Error: Mutex operation failed.\n",
		"Error: Sleep failed.\n",
		"Error: Output write failed.\n"
	};

	return (messages[status]);
}

t_status	print_return_error(t_status status)
{
	const char	*message;
	size_t		length;

	if (status > STATUS_STOPPED && status < ERR_TOTAL)
	{
		message = error_message(status);
		length = 0;
		while (message[length])
			length++;
		if (write_guaranteed(STDERR_FILENO, message, length) != STATUS_OK)
			return (ERR_WRITE);
	}
	return (status);
}
