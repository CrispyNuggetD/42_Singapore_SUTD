/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_splitter_empty_as_dot.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 15:07:34 by hnah              #+#    #+#             */
/*   Updated: 2026/09/21 16:46:35 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static size_t	count_entries(char *path)
{
	size_t	count;

	count = 1;
	while (*path)
	{
		if (*path == ':')
			count++;
		path++;
	}
	return (count);
}

static int	fill_entries_with_empty_as_dot(char **path_entries, char *path)
{
	char	*start;
	size_t	i;

	i = 0;
	while (1)
	{
		start = path;
		while (*path && *path != ':')
			path++;
		if (path == start)
			path_entries[i] = ft_strdup(".");
		else
			path_entries[i] = ft_substr(start, 0, path - start);
		if (!path_entries[i++])
			return (0);
		if (!*path)
			return (1);
		path++;
	}
}

char	**split_path_empty_as_dot(char *path)
{
	char	**path_entries;

	path_entries = ft_calloc(count_entries(path) + 1, sizeof(char *));
	if (!path_entries)
		return (NULL);
	if (!fill_entries_with_empty_as_dot(path_entries, path))
	{
		ryker_ft_free_str_array(path_entries);
		return (NULL);
	}
	return (path_entries);
}
