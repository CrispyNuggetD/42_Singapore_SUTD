/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   solve.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 19:34:26 by hnah              #+#    #+#             */
/*   Updated: 2025/08/03 19:34:49 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//Final approach chosen and credits code contributed by zhoon

int	check_top_bottom(int grid[4][4], int *clues);
int	check_left_right(int grid[4][4], int *clues);

int	visible_buildings(int *line)
{
	int	i;
	int	max;
	int	count;

	max = 0;
	count = 0;
	i = 0;
	while (i < 4)
	{
		if (line[i] > max)
		{
			max = line[i];
			count++;
		}
		i++;
	}
	return (count);
}

int	check_duplicates(int grid[4][4], int row, int col, int num)
{
	int	i;

	i = 0;
	while (i < 4)
	{
		if (grid[row][i] == num || grid[i][col] == num)
			return (0);
		i++;
	}
	return (1);
}

int	solve(int grid[4][4], int *clues, int pos)
{
	int	row;
	int	col;
	int	num;

	row = pos / 4;
	col = pos % 4;
	num = 1;
	if (pos == 16)
	{
		if (check_top_bottom(grid, clues) && check_left_right(grid, clues))
			return (1);
		return (0);
	}
	while (num <= 4)
	{
		if (check_duplicates(grid, row, col, num))
		{
			grid[row][col] = num;
			if (solve(grid, clues, pos + 1))
				return (1);
			grid[row][col] = 0;
		}
		num++;
	}
	return (0);
}
