/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 21:10:43 by hnah              #+#    #+#             */
/*   Updated: 2026/10/24 22:19:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_BONUS_H
# define PHILO_BONUS_H

# include <errno.h>
# include <fcntl.h>
# include <limits.h>
# include <pthread.h>
# include <semaphore.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

/* From mandatory: input settings and millisecond polling interval. */
# define EAT_FOREVER -1
# define WAIT_CHECK_MICROSECONDS 100

# define SEM_FORKS_NAME "/hnah_philo_forks"
# define SEM_PAIR_NAME "/hnah_philo_take_pair"
# define SEM_PRINT_NAME "/hnah_philo_printing"

/* Child exit codes are separate from the parent's function status enum. */
# define CHILD_DONE 0
# define CHILD_DIED 1
# define CHILD_ERROR 2

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
	ERR_TIME,
	ERR_SLEEP,
	ERR_WRITE,
	ERR_FORK,
	ERR_WAITPID,
	ERR_KILL,
	ERR_NOT_IMPLEMENTED,
	ERR_SEM_OPEN,
	ERR_SEM_CLOSE,
	ERR_SEM_UNLINK,
	ERR_TOTAL
}	t_status;

typedef struct s_config
{
	int		count;
	long	time_to_die;
	long	time_to_eat;
	long	time_to_sleep;
	int		optional_meal_goal;
}	t_config;

/*
** Parent-owned bookkeeping. Children inherit copies, not shared updates.
** count is array capacity; alive counts children not yet reaped.
** A positive PID identifies an unreaped child; zero means unused or reaped.
*/
typedef struct s_children
{
	pid_t	*pids;
	int		count;
	int		alive;
}	t_children;

typedef struct s_sim_bonus
{
	t_config	config;
	t_children	children;
	sem_t		*forks;
	sem_t		*take_pair;
	sem_t		*printing;
}	t_sim_bonus;

t_status	parse_input(int argc, char **argv, t_config *config);
t_status	ft_pos_atoi_status(const char *nptr, int *out_value);
t_status	write_guaranteed(int fd, const char *buffer, size_t length);
t_status	print_return_error(t_status status);
long		now_ms(void);
t_status	wait_ms(long duration_ms);
t_status	init_bonus(t_sim_bonus *sim);
t_status	start_bonus(t_sim_bonus *sim);
t_status	supervise_children(t_sim_bonus *sim);
int			run_philo_bonus(t_sim_bonus *sim, int id);
t_status	cleanup_bonus(t_sim_bonus *sim, t_status status);
t_status	init_children(t_children *children, int count);
t_status	fork_philo(t_children *children, int index, pid_t *pid);
t_status	wait_next_child(t_children *children, int *exit_code);
t_status	cleanup_children(t_children *children);

#endif
