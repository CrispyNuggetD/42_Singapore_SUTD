/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chris_clue_deduction.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 16:37:59 by hnah              #+#    #+#             */
/*   Updated: 2025/08/03 18:10:38 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

void	zero_out_row_col(int grid[9][9][9], int x, int y, int n)
{
	int	j;

	j = 0;
	while (j < n)
	{
		if (j != y)
			grid[x][j][n - 1] = 0;
		if (j != x)
			grid[j][y][n - 1] = 0;
		j++;
	}
}

void	apply_highest_top(int *inputs, int grid[9][9][9], int *size)
{
	int	n;
	int	i;
	int	row;

	n = *size;
	i = 0;
	row = 0;
	while (i < n)
	{
		if (inputs[i] == 1)
		{
			grid[i][row][n - 1] = 1;
			zero_out_row_col(grid, i, row, n);
		}
		i++;
	}
}

void	apply_highest_left(int *inputs, int grid[9][9][9], int *size)
{
	int	n;
	int	i;
	int	col;

	n = *size;
	i = 0;
	col = 0;
	while (i < n)
	{
		if (inputs[2 * n + i] == 1)
		{
			grid[col][i][n - 1] = 1;
			zero_out_row_col(grid, col, i, n);
		}
		i++;
	}
}

void	apply_highest_bottom_pair(int *inputs, int grid[9][9][9], int *size)
{
	int	n;
	int	i;
	int	row;

	n = *size;
	i = 0;
	while (i < n)
	{
		if (inputs[i] + inputs[i + n] == n + 1)
		{
			if (inputs[i] < inputs[i + n])
				row = inputs[i] - 1;
			else
				row = n - inputs[i + n];
			grid[i][row][n - 1] = 1;
			zero_out_row_col(grid, i, row, n);
		}
		i++;
	}
}

void	apply_highest_right_pair(int *inputs, int grid[9][9][9], int *size)
{
	int	n;
	int	i;
	int	col;

	n = *size;
	i = 0;
	while (i < n)
	{
		if (inputs[2 * n + i] + inputs[3 * n + i] == n + 1)
		{
			if (inputs[2 * n + i] < inputs[3 * n + i])
				col = inputs[2 * n + i] - 1;
			else
				col = n - inputs[3 * n + i];
			grid[col][i][n - 1] = 1;
			zero_out_row_col(grid, col, i, n);
		}
		i++;
	}
}
