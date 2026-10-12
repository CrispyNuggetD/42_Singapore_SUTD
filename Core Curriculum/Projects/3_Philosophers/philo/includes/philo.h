/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 21:10:43 by hnah              #+#    #+#             */
/*   Updated: 2026/10/12 18:06:27 by hnah             ###   ########.fr       */
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

typedef enum e_status
{
	STATUS_OK,
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
	ERR_TOTAL
}	t_status;

typedef struct s_locked_var
{
	void			*var;
	pthread_mutex_t	lock;
}	t_locked_var;

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
** left_fork, right_fork: pointers to mutexes in the fork array.
** last_meal: LAST meal START time, in milliseconds.
** meals_eaten: number of completed meals.
** meal_mutex: to protect last_meal and meals_eaten.
** sim: pointer to the shared simulation.
*/
typedef struct s_philo
{
	int				id;
	pthread_t		thread_handle;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	long			last_meal;
	int				meals_eaten;
	pthread_mutex_t	meal_mutex;
	t_sim			*sim;
}	t_philo;

/*
** The runtime fields below hold shared simulation state:
** start_time: simulation start timestamp, in milliseconds.
** is_stop: locked stop flag; 0 while running, 1 when stopping.
** print_mutex: coordinates logging.
*/
typedef struct s_runtime
{
	long			start_time;
	t_locked_var	is_stop;
	pthread_mutex_t	print_mutex;
}	t_runtime;

/*
** The resources below are owned by the simulation:
** forks, philos: allocated arrays, each with config.count elements.
** forks_init, meals_init: mutexes safe to destroy.
** threads_to_join: count of started threads still needing a join.
** init_sim initializes resources; start_philos tracks started threads.
** cleanup_resources joins threads, destroys mutexes, and frees arrays.
*/
typedef struct s_resources
{
	pthread_mutex_t	*forks;
	t_philo			*philos;
	int				forks_init;
	int				meals_init;
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
int			cleanup_resources(t_resources *res, t_status status);
t_status	prepare_to_eat(t_philo *philo);
t_status	philo_eat(t_philo *philo);
t_status	philo_wait(t_philo *philo, long duration_ms);
t_status	start_philos(t_sim *sim);
t_status	join_philos(t_resources *res);
void		*philo_routine(void *argument);
long		now_ms(void);

#endif
