/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_sort_input.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 17:06:20 by hnah              #+#    #+#             */
/*   Updated: 2026/10/01 17:32:41 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

/* Accumulate negatively so INT_MIN never needs to be negated. */
static int	read_integer(char **str, int *value)
{
	int	negative;
	int	limit;
	int	digit;

	negative = (**str == '-');
	if (**str == '-' || **str == '+')
		(*str)++;
	limit = -INT_MAX;
	if (negative)
		limit = INT_MIN;
	*value = 0;
	if (!ft_isdigit(**str))
		return (ERROR);
	while (ft_isdigit(**str))
	{
		digit = *(*str)++ - '0';
		if (*value < limit / 10 || (*value == limit / 10
				&& digit > -(limit % 10)))
			return (ERROR);
		*value = *value * 10 - digit;
	}
	if (!negative)
		*value = -*value;
	return (SUCCESS);
}

static int	store_integer(int *values, int count, int value)
{
	int	i;

	if (!values)
		return (SUCCESS);
	i = 0;
	while (i < count)
	{
		if (values[i++] == value)
			return (ERROR);
	}
	values[count] = value;
	return (SUCCESS);
}

/* NULL values validates/counts; otherwise caller provides enough capacity. */
int	count_int_in_str(char *str, int *count, int *values)
{
	int	value;

	while (*str == ' ')
		str++;
	if (!*str)
		return (ERROR);
	while (*str)
	{
		if (*count == INT_MAX - 1 || read_integer(&str, &value))
			return (ERROR);
		if (*str && *str != ' ')
			return (ERROR);
		if (store_integer(values, *count, value))
			return (ERROR);
		(*count)++;
		while (*str == ' ')
			str++;
	}
	return (SUCCESS);
}
