/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_error_cleanup.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 21:06:49 by hnah              #+#    #+#             */
/*   Updated: 2026/10/12 14:33:18 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** Caller must allocate philos and initialize fork mutexes first.
** Initialize all philosopher records before starting any threads.
*/
t_status	init_philos(t_sim *sim)
{
	t_resources	*res;
	t_philo		*philo;

	res = &sim->resources;
	while (res->meals_init < sim->config.count)
	{
		philo = &res->philos[res->meals_init];
		philo->id = res->meals_init + 1;
		philo->left_fork = &res->forks[res->meals_init];
		philo->right_fork = &res->forks[(res->meals_init + 1)
			% sim->config.count];
		philo->last_meal = sim->runtime.start_time;
		philo->meals_eaten = 0;
		if (pthread_mutex_init(&philo->meal_mutex, NULL) != 0)
			return (ERR_MUTEX_INIT);
		philo->sim = sim;
		res->meals_init++;
	}
	return (STATUS_OK);
}

static t_status	init_forks(t_sim *sim)
{
	t_resources	*res;

	res = &sim->resources;
	while (res->forks_init < sim->config.count)
	{
		if (pthread_mutex_init(&res->forks[res->forks_init], NULL) != 0)
			return (ERR_MUTEX_INIT);
		res->forks_init++;
	}
	return (STATUS_OK);
}

t_status	init_sim(t_sim *sim)
{
	t_status	status;
	t_resources	*res;

	sim->runtime.start_time = now_ms();
	if (sim->runtime.start_time == -1)
		return (ERR_TIME);
	res = &sim->resources;
	res->forks = malloc(sizeof(pthread_mutex_t) * sim->config.count);
	if (res->forks == NULL)
		return (ERR_MALLOC);
	res->philos = malloc(sizeof(t_philo) * sim->config.count);
	if (res->philos == NULL)
		return (ERR_MALLOC);
	status = init_forks(sim);
	if (status != STATUS_OK)
		return (status);
	return (init_philos(sim));
}

/*
** Join started threads before destroying mutexes or freeing their data.
** On join failure, retain resources that a running thread could still use.
*/
int	cleanup(t_sim *sim, t_status status)
{
	t_resources	*res;

	res = &sim->resources;
	while (res->threads_to_join > 0)
	{
		if (pthread_join(res->philos[res->threads_to_join - 1].thread_handle,
				NULL) != 0)
			return (print_return_error(ERR_THREAD_JOIN) != STATUS_OK);
		res->threads_to_join--;
	}
	while (res->meals_init > 0)
	{
		res->meals_init--;
		pthread_mutex_destroy(&res->philos[res->meals_init].meal_mutex);
	}
	while (res->forks_init > 0)
		pthread_mutex_destroy(&res->forks[--res->forks_init]);
	free(res->philos);
	free(res->forks);
	res->philos = NULL;
	res->forks = NULL;
	return (print_return_error(status) != STATUS_OK);
}

/*
** printf writes to stdout, so I use write to send errors to stderr here.
** This function reports the error and returns its status.
*/
t_status	print_return_error(t_status status)
{
	static const char	*messages[] = {
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
		"Error: Thread join failed.\n"
	};
	size_t				length;

	if (status > STATUS_OK && status < ERR_TOTAL)
	{
		length = 0;
		while (messages[status][length])
			length++;
		write(STDERR_FILENO, messages[status], length);
	}
	return (status);
}
