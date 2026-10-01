/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_storage.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:42:07 by hnah              #+#    #+#             */
/*   Updated: 2026/10/01 18:47:46 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	parse_arguments(char **argv, int *count, int *values)
{
	*count = 0;
	while (*argv)
	{
		if (count_int_in_str(*argv++, count, values))
			return (ERROR);
	}
	return (SUCCESS);
}

/* Count tokens before allocating; quoted arguments may hold many values. */
int	parse_input(char **argv, t_circle_buf *a, t_circle_buf *b)
{
	int	count;

	if (parse_arguments(argv, &count, NULL))
		return (ERROR);
	if (cbuf_allocate_ab(a, b, count))
		return (ERROR);
	if (parse_arguments(argv, &count, cbuf_data(b)))
		return (ERROR);
	return (rank_values(count, cbuf_data(b), cbuf_data(a)));
}
