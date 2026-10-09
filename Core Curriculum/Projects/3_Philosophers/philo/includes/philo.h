#ifndef PHILO_H
# define PHILO_H

# include <limits.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

typedef enum e_error
{
	ERR_NONE,
	ERR_USAGE,
	ERR_NEGATIVE,
	ERR_INVALID_INTEGER,
	ERR_INTEGER_OVERFLOW,
	ERR_NON_POSITIVE
}	t_error;

typedef struct s_locked_var
{
	void			*var;
	pthread_mutex_t	lock;
}	t_locked_var;

typedef struct s_sim	t_sim;

typedef struct s_philo
{
	int				id; // Philosopher number: 1 to N
	pthread_t		thread; // Handle used for pthread_create/join
	pthread_mutex_t	*left_fork; // Points to a mutex in the fork array
	pthread_mutex_t	*right_fork; // Points to the other fork mutex
	long			last_meal; // Last meal start time, in milliseconds
	int				meals_eaten; // Number of completed meals
	pthread_mutex_t	meal_mutex; // Protects last_meal and meals_eaten
	t_sim			*sim; // Pointer to the shared simulation
}	t_philo;

typedef struct s_sim
{
	int				count; // Number of philosophers and forks
	long			time_to_die; // Maximum time since meal start, in ms
	long			time_to_eat; // Eating duration, in ms
	long			time_to_sleep; // Sleeping duration, in ms
	int				meal_goal; // Optional meal quota; -1 means omitted
	long			start_time; // Shared simulation start time, in ms
	t_locked_var	is_stop; // 0 while running, 1 when stopping
	pthread_mutex_t	print_mutex; // Coordinates logging
	pthread_mutex_t	*forks; // Allocated array of count fork mutexes
	t_philo			*philos; // Allocated array of count philosophers
}	t_sim;

t_error	parse_input(int argc, char **argv, t_sim *sim);
t_error	ft_pos_atoi_status(const char *nptr, int *out_value);
void	print_error(t_error error);
long	now_ms(void);

#endif
