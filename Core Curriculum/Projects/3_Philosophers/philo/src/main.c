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