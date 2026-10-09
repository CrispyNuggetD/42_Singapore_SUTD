/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_input.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 21:10:43 by hnah              #+#    #+#             */
/*   Updated: 2026/10/09 22:11:29 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

t_error	parse_input(int argc, char **argv, t_sim *sim)
{
	int		values[5];
	int		i;
	t_error	error;

	if (argc < 5 || argc > 6)
		return (ERR_USAGE);
	i = 0;
	while (i < argc - 1)
	{
		error = ft_pos_atoi_status(argv[i + 1], &values[i]);
		if (error != ERR_NONE)
			return (error);
		if (values[i] == 0)
			return (ERR_NON_POSITIVE);
		i++;
	}
	sim->count = values[0];
	sim->time_to_die = values[1];
	sim->time_to_eat = values[2];
	sim->time_to_sleep = values[3];
	sim->meal_goal = -1;
	if (argc == 6)
		sim->meal_goal = values[4];
	return (ERR_NONE);
}

t_error	ft_pos_atoi_status(const char *nptr, int *out_value)
{
	int	number;

	number = 0;
	while (*nptr == ' ' || *nptr == '\t' || *nptr == '\n'
		|| *nptr == '\r' || *nptr == '\v' || *nptr == '\f')
		nptr++;
	if (*nptr == '-')
		return (ERR_NEGATIVE);
	if (*nptr == '+')
		nptr++;
	if (*nptr < '0' || *nptr > '9')
		return (ERR_INVALID_INTEGER);
	while (*nptr >= '0' && *nptr <= '9')
	{
		if (number > (INT_MAX - (*nptr - '0')) / 10)
			return (ERR_INTEGER_OVERFLOW);
		number = number * 10 + (*nptr++ - '0');
	}
	if (*nptr != '\0')
		return (ERR_INVALID_INTEGER);
	*out_value = number;
	return (ERR_NONE);
}

void	print_error(t_error error)
{
	static const char	*messages[] = {
		"",
		("Usage: ./philo <number_of_philosophers> <time_to_die> "
		"<time_to_eat> <time_to_sleep> [meal_goal]\n"),
		"Error: Negative integers are not allowed.\n",
		"Error: Invalid integer input.\n",
		"Error: Integer input exceeds INT_MAX.\n",
		"Error: Arguments must be greater than zero.\n"
	};
	size_t				length;

	if (error <= ERR_NONE || error > ERR_NON_POSITIVE)
		return ;
	length = 0;
	while (messages[error][length])
		length++;
	write(STDERR_FILENO, messages[error], length);
}
