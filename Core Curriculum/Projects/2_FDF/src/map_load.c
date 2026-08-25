/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_load.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 01:06:34 by hnah              #+#    #+#             */
/*   Updated: 2026/08/26 01:37:04 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"
#include "libft.h"
#include "get_next_line.h"
#include <fcntl.h>
#include <stdint.h>

int	allocate_map(t_map *map)
{
	if (map->width <= 0 || map->height <= 0)
		return (1);
	if ((size_t)map->width > SIZE_MAX / sizeof(t_point)
		/ (size_t)map->height)
		return (1);
	map->points = malloc(sizeof(t_point) * (size_t)map->width
			* (size_t)map->height);
	if (map->points == NULL)
		return (1);
	return (0);
}

static int	fill_row(t_map *map, char **words, int y)
{
	int	x;
	int	index;

	x = 0;
	while (x < map->width && words[x] != NULL)
	{
		index = y * map->width + x;
		map->points[index].x = x;
		map->points[index].y = y;
		/* Point colours after the comma will be parsed separately later. */
		map->points[index].z = ft_atoi(words[x]);
		x++;
	}
	if (x != map->width || words[x] != NULL)
		return (1);
	return (0);
}

static int	load_error(int fd, char *line, t_map *map)
{
	free(line);
	close(fd);
	free(map->points);
	map->points = NULL;
	return (1);
}

static int	read_rows(int fd, t_map *map)
{
	char	*line;
	char	**words;
	int		y;

	y = 0;
	line = get_next_line(fd);
	while (line != NULL && y < map->height)
	{
		words = split_map_line(line);
		if (words == NULL || fill_row(map, words, y) != 0)
		{
			free_words(words);
			return (load_error(fd, line, map));
		}
		free_words(words);
		free(line);
		y++;
		line = get_next_line(fd);
	}
	if (line != NULL || y != map->height)
		return (load_error(fd, line, map));
	close(fd);
	return (0);
}

int	read_map(const char *filename, t_map *map)
{
	int	fd;

	if (map_dimensions(filename, map) != 0 || allocate_map(map) != 0)
		return (1);
	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		free(map->points);
		map->points = NULL;
		return (1);
	}
	return (read_rows(fd, map));
}
