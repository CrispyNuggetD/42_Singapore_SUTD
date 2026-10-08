#include "philo.h"

/*
** Allowed external functions (subject v13.0; not all are required).
** Memory: memset(), malloc(), free()
** Output: printf(), write()
** Time: usleep(), gettimeofday()
** Threads: pthread_create(), pthread_detach(), pthread_join()
** Mutexes: pthread_mutex_init(), pthread_mutex_destroy(),
** pthread_mutex_lock(), pthread_mutex_unlock()
*/

int	main(int argc, char **argv)
{
	thread_t	*threads = malloc(sizeof(pthread_t) * data->num_philosophers);

	parse_input(argc, argv);
	initialize_data();
	while (1)
	{
		current_time - last_meal >= time_to_die? printf("Philosopher %d has died\n", i), exit(1) : 0;
	}
	create_threads(threads);
	// Add fork acquisition and the eating cycle.
	// Add protected meal tracking and death monitoring.
	// Add coordinated stopping, responsive sleeps, and cleanup.
	cleanup(threads);
	return (0);
}

// Use the same clock basis for start_time and last_meal. Initialize mutexes with pthread_mutex_init() before using them; don’t copy initialized mutexes.

/* Philosopher updating their meal time */
pthread_mutex_lock(&philo->meal_mutex);
philo->last_meal = current_time;
pthread_mutex_unlock(&philo->meal_mutex);

/* Monitor reading it */
pthread_mutex_lock(&philo->meal_mutex);
last_meal = philo->last_meal;
pthread_mutex_unlock(&philo->meal_mutex);


#include <pthread.h>
#include <stdio.h>

typedef struct s_worker
{
	int	id;
}	t_worker;

void	*routine(void *argument)
{
	t_worker	*worker;

	worker = argument;
	printf("Hello from worker %d\n", worker->id);
	return (NULL);
}

int	main(void)
{
	pthread_t	thread;
	t_worker	worker;

	worker.id = 1;
	if (pthread_create(&thread, NULL, routine, &worker) != 0)
		return (1);
	printf("Hello from main\n");
	if (pthread_join(thread, NULL) != 0)
		return (1);
	return (0);
}

/* Inside a loop, after initializing the philosopher array */
pthread_create(&philos[i].thread, NULL,
	routine, &philos[i]);

	pthread_mutex_t	lock;

	pthread_mutex_init(&lock, NULL);

pthread_mutex_lock(&lock);
/* Critical section: protected work */
pthread_mutex_unlock(&lock);

pthread_mutex_destroy(&lock);