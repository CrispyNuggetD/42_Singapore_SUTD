/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 19:35:37 by hnah              #+#    #+#             */
/*   Updated: 2025/08/03 19:35:43 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//Final approach chosen and credits code contributed by zhoon

int	visible_buildings(int *line);

void	extract_col(int grid[4][4], int col, int *colarray)
{
	int	i;

	i = 0;
	while (i < 4)
	{
		colarray[i] = grid[i][col];
		i++;
	}
}

void	extract_row(int grid[4][4], int row, int *rowarray)
{
	int	i;

	i = 0;
	while (i < 4)
	{
		rowarray[i] = grid[row][i];
		i++;
	}
}

void	reverse(int *arr)
{
	int	temp;
	int	i;
	int	j;

	i = 0;
	j = 3;
	while (i < j)
	{
		temp = arr[i];
		arr[i] = arr[j];
		arr[j] = temp;
		i++;
		j--;
	}
}

int	check_top_bottom(int grid[4][4], int *clues)
{
	int	line[4];
	int	x;

	x = 0;
	while (x < 4)
	{
		extract_col(grid, x, line);
		if (visible_buildings(line) != clues[x])
			return (0);
		reverse(line);
		if (visible_buildings(line) != clues[x + 4])
			return (0);
		x++;
	}
	return (1);
}

int	check_left_right(int grid[4][4], int *clues)
{
	int	line[4];
	int	x;

	x = 0;
	while (x < 4)
	{
		extract_row(grid, x, line);
		if (visible_buildings(line) != clues[x + 8])
			return (0);
		reverse(line);
		if (visible_buildings(line) != clues[x + 12])
			return (0);
		x++;
	}
	return (1);
}
