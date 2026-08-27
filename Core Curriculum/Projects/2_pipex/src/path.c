/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 00:00:00 by hnah              #+#    #+#             */
/*   Updated: 2026/08/27 00:00:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static char	*path_value(char **envp)
{
	int	i;

	i = 0;
	while (envp && envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			return (envp[i] + 5);
		i++;
	}
	return (NULL);
}

static char	*join_command(char *directory, char *command)
{
	char	*prefix;
	char	*path;

	prefix = ft_strjoin(directory, "/");
	if (!prefix)
		return (NULL);
	path = ft_strjoin(prefix, command);
	free(prefix);
	return (path);
}

static char	*search_directories(char **directories, char *command)
{
	char	*path;
	int		i;

	i = 0;
	while (directories[i])
	{
		path = join_command(directories[i], command);
		if (!path)
			return (NULL);
		if (access(path, F_OK) == 0)
			return (path);
		free(path);
		i++;
	}
	return (NULL);
}

char	*resolve_path(char *command, char **envp)
{
	char	**directories;
	char	*path;
	char	*value;

	if (ft_strchr(command, '/'))
		return (ft_strdup(command));
	value = path_value(envp);
	if (!value)
		return (NULL);
	directories = ft_split(value, ':');
	if (!directories)
		return (NULL);
	path = search_directories(directories, command);
	free_matrix(directories);
	return (path);
}
