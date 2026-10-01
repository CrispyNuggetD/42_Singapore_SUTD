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

static int	has_duplicates(const int *count, const int *values);
static int	is_improper_int(char **str_move, int *count, int sign);
static int	exceed_int_range(char **str_move, const char *int_limit);

int	count_int_in_str(char *str, int *count, int *values)
{
	char	*str_move;
	char	*number_start;
	int		sign;

	while (*str == ' ')
		str++;
	number_start = str;
	sign = 1;
	if (*str == '-')
		sign = -1;
	if (*str == '+' || *str == '-')
		str++;
	if (!ft_isdigit(*str))
		return (ERROR);
	str_move = str;
	if (is_improper_int(&str_move, count, sign))
		return (ERROR);
	values[*count - 1] = ft_atoi(number_start);
	if (has_duplicates(count, values))
		return (ERROR);
	if (*str_move && count_int_in_str(str_move, count, values))
		return (ERROR);
	return (SUCCESS);
}

static int	has_duplicates(const int *count, const int *values)
{
	int	cur_compare;
	int	looping_index;

	cur_compare = *count - 1;
	looping_index = cur_compare - 1;
	while (looping_index >= 0)
	{
		if (values[looping_index] == values[cur_compare])
			return (ERROR);
		looping_index--;
	}
	return (SUCCESS);
}

static int	is_improper_int(char **str_move, int *count, int sign)
{
	const char	*int_limit;

	if (sign > 0)
		int_limit = "2147483647";
	else
		int_limit = "2147483648";
	if (exceed_int_range(str_move, int_limit))
		return (ERROR);
	if (**str_move && **str_move != ' ')
		return (ERROR);
	(*count)++;
	while (**str_move == ' ')
		(*str_move)++;
	if (*count > 500)
		return (ERROR);
	return (SUCCESS);
}

static int	exceed_int_range(char **str_move, const char *int_limit)
{
	int	digits;

	digits = 0;
	while (**str_move && ft_isdigit(**str_move))
	{
		digits++;
		if (digits > 10)
			return (ERROR);
		(*str_move)++;
	}
	if (digits == 10)
	{
		while (digits)
		{
			if (*(*str_move - digits) < int_limit[10 - digits])
				break ;
			else if (*(*str_move - digits) > int_limit[10 - digits])
				return (ERROR);
			digits--;
		}
	}
	return (SUCCESS);
}
