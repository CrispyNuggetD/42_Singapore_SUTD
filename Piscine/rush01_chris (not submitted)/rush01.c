/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rush01.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 19:30:05 by hnah              #+#    #+#             */
/*   Updated: 2025/08/03 18:47:03 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void	apply_highest_top(int *inputs, int grid[9][9][9], int *size);
void	apply_highest_left(int *inputs, int grid[9][9][9], int *size);
void	apply_highest_bottom_pair(int *inputs, int grid[9][9][9], int *size);
void	apply_highest_right_pair(int *inputs, int grid[9][9][9], int *size);

int	get_input_size(int argc, char **argv, int *size, int *inputs_container)
{
	int		i;
	char	*ptr;

	ptr = argv[1];
	i = 0;
	if (argc != 2)
		return (1);
	while (*ptr)
	{
		if ((*ptr - '0' < 1 || *ptr - '0' > 9) && *ptr != ' ')
			return (1);
		if (*ptr == *(ptr - 1))
		{
			printf("test");
			return (1);
		}
		if (*ptr != ' ')
		{
			inputs_container[*size] = *ptr - '0';
			*size += 1;
		}
		ptr++;
	}
	if (*size == 0)
		return (1);
	return (0);
}

int	get_dimension(int *size)
{
	int	i;
	int	found;

	i = 4;
	found = 0;
	while (i <= 9)
	{
		if ((*size / 4 == i) && (*size % i == 0))
		{
			*size = i;
			found = 1;
			break ;
		}
		i++;
	}
	if (found == 0)
		return (1);
	return (0);
}

int	error_checking(int argc, char **argv, int *size, int *inputs_container)
{
	if (get_input_size(argc, argv, size, inputs_container))
		return (1);
	if (get_dimension(size))
		return (1);
	if (argv[1][(*size * 4) * 2 - 1] == ' ')
	{
		printf("test: %i", (*size * 4));
		return (1);
	}
	return (0);
}

int	clue_elimination(int *inputs, int grid[9][9][9], int *size)
{
	apply_highest_top(inputs, grid, size);
	apply_highest_left(inputs, grid, size);
	apply_highest_bottom_pair(inputs, grid, size);
	apply_highest_right_pair(inputs, grid, size);
	return (0);
}

int	main(int argc, char **argv)
{
	int	size;
	int	inputs[36];
	int	i;
	int	grid[9][9][9];

	i = 0;
	size = 0;
	if ((error_checking(argc, argv, &size, inputs)))
	{
		write(1, "Error\n", 6);
		return (1);
	}
	while (i < size * 4)
	{
		printf("%d ", inputs[i]);
		i++;
	}
	clue_elimination(inputs, grid, &size);
	printf("\n");
}
