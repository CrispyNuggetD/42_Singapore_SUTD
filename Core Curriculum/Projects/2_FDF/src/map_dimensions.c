/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_dimensions.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 00:43:39 by hnah              #+#    #+#             */
/*   Updated: 2026/08/26 00:43:39 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"
#include "libft.h"
#include "get_next_line.h"
#include <fcntl.h>

void	free_words(char **words)
{
	int	index;

	if (words == NULL)
		return ;
	index = 0;
	while (words[index] != NULL)
	{
		free(words[index]);
		index++;
	}
	free(words);
}

static int	set_map_width(char *line, t_map *map)
{
	char	**words;
	int		index;

	words = ft_split(line, ' ');
	if (words == NULL)
		return (1);
	index = 0;
	while (words[index] != NULL)
		index++;
	map->width = index;
	free_words(words);
	if (map->width == 0)
		return (1);
	return (0);
}

static int	dimension_error(int fd, char *line)
{
	free(line);
	close(fd);
	return (1);
}

int	map_dimensions(const char *filename, t_map *map)
{
	int		fd;
	char	*line;

	map->width = 0;
	map->height = 0;
	fd = open(filename, O_RDONLY);
	if (fd < 0)
		return (1);
	line = get_next_line(fd);
	while (line != NULL)
	{
		if (map->height == 0 && set_map_width(line, map) != 0)
			return (dimension_error(fd, line));
		map->height++;
		free(line);
		line = get_next_line(fd);
	}
	close(fd);
	if (map->height == 0)
		return (1);
	return (0);
}
