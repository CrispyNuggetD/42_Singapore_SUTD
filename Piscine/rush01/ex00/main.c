/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 19:32:41 by hnah              #+#    #+#             */
/*   Updated: 2025/08/03 19:34:07 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

//Final approach chosen and credits code contributed by zhoon

//Declare external functions from solve.c and print.c
int	solve(int grid[4][4], int *clues, int pos);

void	fill_grid(int grid[4][4])
{
	int	i;
	int	j;

	i = 0;
	while (i < 4)
	{
		j = 0;
		while (j < 4)
		{
			grid[i][j] = 0;
			j++;
		}
		i++;
	}
}

//Function to print the grid on terminal
void	display_grid(int grid[4][4])
{
	int		i;
	int		j;
	char	a;

	i = 0;
	while (i < 4)
	{
		j = 0;
		while (j < 4)
		{
			a = grid[i][j] + '0';
			write(1, &a, 1);
			if (j < 3)
			{
				write(1, " ", 1);
			}
			j++;
		}
		write(1, "\n", 1);
		i++;
	}
}

//Count the number of inputs under **argv and check for errors
int	check_errors(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (i % 2 == 0 && (str[i] < '1' || str[i] > '4'))
		{
			return (0);
		}
		else if (i % 2 == 1 && str[i] != ' ')
		{
			return (0);
		}
		i++;
	}
	if (i != 31)
		return (0);
	return (1);
}

//Execute the main program tgt with command line argument
int	main(int argc, char **argv)
{
	int	grid[4][4];
	int	clues[16];
	int	j;
	int	i;

	j = 0;
	i = 0;
	fill_grid(grid);
	if (argc != 2 || !(check_errors(argv[1])))
	{
		write(1, "Error\n", 6);
		return (1);
	}
	while (j < 16)
	{
		clues[j] = argv[1][i] - '0';
		j++;
		i += 2;
	}
	if (solve(grid, clues, 0))
		display_grid(grid);
	else
		write(1, "Error\n", 6);
	return (0);
}
