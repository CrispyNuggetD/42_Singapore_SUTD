#include <pthread.h>
#include <stdio.h>
#include "philo.h"


void	*increment(void *argument)
{
	t_locker	*locker;
	int			i;

	locker = argument;
	i = 0;
	while (i < 100000)
	{
		pthread_mutex_lock(&locker->lock);
		*(int *)locker->var += 1;
		pthread_mutex_unlock(&locker->lock);
		i++;
	}
	return (NULL);
}

int	main(void)
{
	t_shared	shared;
	pthread_t	first;
	pthread_t	second;

	shared.counter = 0;
	if (pthread_mutex_init(&shared.lock, NULL) != 0)
		return (1);
	if (pthread_create(&first, NULL, increment, &shared) != 0)
	{
		pthread_mutex_destroy(&shared.lock);
		return (1);
	}
	if (pthread_create(&second, NULL, increment, &shared) != 0)
	{
		pthread_join(first, NULL);
		pthread_mutex_destroy(&shared.lock);
		return (1);
	}
	pthread_join(first, NULL);
	pthread_join(second, NULL);
	printf("%d\n", shared.counter);
	pthread_mutex_destroy(&shared.lock);
	return (0);
}