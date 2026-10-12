/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_output.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 21:10:43 by hnah              #+#    #+#             */
/*   Updated: 2026/10/12 14:02:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

t_status	parse_input(int argc, char **argv, t_config *config)
{
	int			values[5];
	int			i;
	t_status	status;

	if (argc < 5 || argc > 6)
		return (ERR_USAGE);
	i = 0;
	while (i < argc - 1)
	{
		status = ft_pos_atoi_status(argv[i + 1], &values[i]);
		if (status != STATUS_OK)
			return (status);
		i++;
	}
	config->count = values[0];
	config->time_to_die = values[1];
	config->time_to_eat = values[2];
	config->time_to_sleep = values[3];
	config->optional_meal_goal = EAT_FOREVER;
	if (argc == 6)
		config->optional_meal_goal = values[4];
	return (STATUS_OK);
}

t_status	ft_pos_atoi_status(const char *nptr, int *out_value)
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
	if (number == 0)
		return (ERR_NON_POSITIVE);
	*out_value = number;
	return (STATUS_OK);
}

/*
** Adapted from my Push_swap ft_printf helper, without its print context.
** Finish partial writes; report failure if write errors or makes no progress.
*/
t_status	write_guaranteed(int fd, const char *buffer, size_t length)
{
	ssize_t	written;

	while (length > 0)
	{
		written = write(fd, buffer, length);
		if (written <= 0)
			return (ERR_WRITE);
		buffer += written;
		length -= (size_t)written;
	}
	return (STATUS_OK);
}
