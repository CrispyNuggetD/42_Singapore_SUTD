/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   semaphore_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 17:34:30 by hnah              #+#    #+#             */
/*   Updated: 2026/11/02 17:36:44 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/* Store the opened handle in the caller's semaphore pointer. */
t_status	open_semaphore(sem_t **sem, const char *name,
		unsigned int value)
{
	*sem = sem_open(name, O_CREAT | O_EXCL, 0600, value);
	if (*sem == SEM_FAILED)
		return (ERR_SEM_OPEN);
	return (STATUS_OK);
}

/* From Pipex file_unique: append a decimal candidate suffix in place. */
static void	append_numeric_suffix(char *name, int number, int *length)
{
	if (number >= 10)
		append_numeric_suffix(name, number / 10, length);
	name[(*length)++] = '0' + number % 10;
	name[*length] = '\0';
	return ;
}

/* Only this child needs the handle; remove its name before starting threads. */
static t_status	unlink_meal_semaphore(sem_t **sem, const char *name)
{
	if (sem_unlink(name) == 0)
		return (STATUS_OK);
	if (sem_close(*sem) == -1)
		return (ERR_SEM_CLOSE);
	*sem = NULL;
	return (ERR_SEM_UNLINK);
}

/* Retry name collisions only; other sem_open failures propagate immediately. */
t_status	open_unique_meal_semaphore(t_philo_bonus *philo)
{
	char		name[sizeof(SEM_MEAL_NAME) + 10];
	int			number;
	int			length;
	t_status	status;

	number = 0;
	while (number < INT_MAX)
	{
		length = 0;
		while (SEM_MEAL_NAME[length])
		{
			name[length] = SEM_MEAL_NAME[length];
			length++;
		}
		append_numeric_suffix(name, number, &length);
		status = open_semaphore(&philo->meal_state_lock, name, 1);
		if (status == STATUS_OK)
			return (unlink_meal_semaphore(&philo->meal_state_lock, name));
		if (errno != EEXIST)
			return (status);
		number++;
	}
	return (ERR_SEM_OPEN);
}

/* Signals may interrupt a wait without acquiring a semaphore unit. */
t_status	wait_semaphore(sem_t *sem)
{
	while (sem_wait(sem) == -1)
	{
		if (errno != EINTR)
			return (ERR_SEM_WAIT);
	}
	return (STATUS_OK);
}
