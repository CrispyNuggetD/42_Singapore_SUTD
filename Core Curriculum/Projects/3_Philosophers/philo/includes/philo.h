/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 21:10:43 by hnah              #+#    #+#             */
/*   Updated: 2026/10/20 16:52:55 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <limits.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

/*
 * EAT_FOREVER below disables the meal quota.
 * Their stomachs are a while (1); they forgot the
 * break; (get it?)
 */
# define EAT_FOREVER -1

/* Wait between checks: 100 microseconds = 0.1 milliseconds. */
# define WAIT_CHECK_MICROSECONDS 100

/*
** STATUS_OK: operation succeeded; STATUS_STOPPED: normal early completion.
** A returned STATUS_STOPPED does not itself change the shared all_stop flag.
** Passing it as a request to check_or_stop_sim DOES request global stopping.
** ERR_* values describe operation failures; first_error keeps the first one.
*/
typedef enum e_status
{
	STATUS_OK,
	STATUS_STOPPED,
	ERR_USAGE,
	ERR_NEGATIVE,
	ERR_INVALID_INTEGER,
	ERR_INTEGER_OVERFLOW,
	ERR_NON_POSITIVE,
	ERR_MALLOC,
	ERR_MUTEX_INIT,
	ERR_TIME,
	ERR_THREAD_CREATE,
	ERR_THREAD_JOIN,
	ERR_MUTEX_DESTROY,
	ERR_MUTEX,
	ERR_SLEEP,
	ERR_WRITE,
	ERR_INVALID_LOG_STATE,
	ERR_TOTAL
}	t_status;

typedef enum e_philo_state
{
	LOG_FORK,
	LOG_EATING,
	LOG_SLEEPING,
	LOG_THINKING,
	LOG_DIED,
	LOG_TOTAL
}	t_philo_state;

/*
** The configuration below is populated by parse_input:
** count: number of philosophers and forks.
** time_to_die: maximum time since meal start, in milliseconds.
** time_to_eat, time_to_sleep: activity durations, in milliseconds.
** optional_meal_goal: meal quota; EAT_FOREVER means omitted.
*/
typedef struct s_config
{
	int		count;
	long	time_to_die;
	long	time_to_eat;
	long	time_to_sleep;
	int		optional_meal_goal;
}	t_config;

typedef struct s_sim	t_sim;

/*
** Below describe one philosopher:
** id: philosopher number, from 1 to N.
** thread_handle: handle used by pthread_create and pthread_join.
** first_fork_lock, second_fork_lock: fork pointers in acquisition order.
** meal_state_lock: protects reads and writes of the next two fields.
** last_meal: LAST meal START time, in milliseconds.
** meals_eaten: number of completed meals.
** sim: pointer to the shared simulation.
*/
typedef struct s_philo
{
	int				id;
	pthread_t		thread_handle;
	pthread_mutex_t	*first_fork_lock;
	pthread_mutex_t	*second_fork_lock;
	pthread_mutex_t	meal_state_lock;
	long			last_meal;
	int				meals_eaten;
	t_sim			*sim;
}	t_philo;

/*
** The runtime fields below hold shared simulation state:
** start_time: simulation start timestamp, in milliseconds.
** all_stop: shared stop flag; 0 while running, 1 when stopping.
** first_error: first recorded ERR_*; STATUS_OK means no error recorded.
** all_stop_lock: protects all_stop and first_error.
** printing_lock: coordinates logging.
*/
typedef struct s_runtime
{
	long			start_time;
	int				all_stop;
	int				completed_meals;
	t_status		first_error;
	pthread_mutex_t	all_stop_lock;
	pthread_mutex_t	printing_lock;
	pthread_mutex_t	completed_meals_lock;
}	t_runtime;

/*
** The resources below are owned by the simulation:
** all_forks_mutexes: allocated array of config.count fork mutexes.
** philos: allocated array of config.count philosopher records.
** forks_init_count: number of initialized fork mutexes safe to destroy.
** meals_init_count: number of initialized meal mutexes safe to destroy.
** stop_lock_initialized, printing_lock_initialized: shared-lock success flags.
** threads_to_join: count of started threads still needing a join.
** init_sim initializes resources; start_sim starts threads and runs the monitor.
** cleanup_resources joins threads, destroys mutexes, and frees arrays.
*/
typedef struct s_resources
{
	pthread_mutex_t	*all_forks_mutexes;
	t_philo			*philos;
	int				forks_init_count;
	int				meals_init_count;
	int				stop_lock_initialized;
	int				printing_lock_initialized;
	int				completed_meals_lock_initialized;
	int				threads_to_join;
}	t_resources;

/*
** The simulation below groups settings, shared state, and owned resources:
** config: validated settings populated by parse_input.
** runtime: clock and shared state; init_sim sets start_time.
** resources: arrays and counters used by setup, threads, and cleanup.
*/
typedef struct s_sim
{
	t_config	config;
	t_runtime	runtime;
	t_resources	resources;
}	t_sim;

t_status	parse_input(int argc, char **argv, t_config *config);
t_status	ft_pos_atoi_status(const char *nptr, int *out_value);
t_status	print_return_error(t_status status);
t_status	write_guaranteed(int fd, const char *buffer, size_t length);
t_status	init_philos(t_sim *sim);
t_status	init_sim(t_sim *sim);
int			cleanup_resources(t_resources *res, t_runtime *run,
				t_status status);
t_status	release_forks(t_philo *philo);
t_status	take_forks(t_philo *philo, pthread_mutex_t *first,
				pthread_mutex_t *second);
int			is_even_round(t_philo *philo);
t_status	swap_fork_if_even_round(t_philo *philo,
				pthread_mutex_t **first, pthread_mutex_t **second);
t_status	prepare_to_eat(t_philo *philo);
t_status	philo_eat(t_philo *philo);
t_status	check_or_stop_sim(t_sim *sim, t_status request);
t_status	philo_wait(t_sim *sim, long duration_ms);
t_status	start_sim(t_sim *sim);
t_status	monitor_sim(t_sim *sim);
t_status	log_philo_state(t_philo *philo, t_philo_state state);
t_status	join_philos(t_resources *res);
void		*philo_routine(void *argument);
long		now_ms(void);

#endif
