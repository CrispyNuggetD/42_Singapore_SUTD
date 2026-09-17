/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_checker.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 19:10:52 by hnah              #+#    #+#             */
/*   Updated: 2026/09/17 22:14:36 by hnah             ###   ########.fr       */
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

static t_path_result	search_directories(char **directories, char *command,
		char **path)
{
	int	i;

	i = 0;
	while (directories[i])
	{
		*path = join_command(directories[i], command);
		if (!*path)
			return (PATH_ERROR);
		if (access(*path, F_OK) == 0)
			return (PATH_FOUND);
		free(*path);
		*path = NULL;
		i++;
	}
	return (PATH_NOT_FOUND);
}

static t_path_result	copy_command_path(char *command, char **path)
{
	*path = ft_strdup(command);
	if (!*path)
	{
		errno = ENOMEM;
		return (PATH_ERROR);
	}
	return (PATH_FOUND);
}

t_path_result	resolve_path(char *command, char **envp, char **path)
{
	char			**directories;
	char			*value;
	t_path_result	result;

	*path = NULL;
	if (ft_strchr(command, '/'))
		return (copy_command_path(command, path));
	value = path_value(envp);
	if (!value)
		return (PATH_NOT_FOUND);
	directories = ft_split(value, ':');
	if (!directories)
	{
		errno = ENOMEM;
		return (PATH_ERROR);
	}
	result = search_directories(directories, command, path);
	ryker_ft_free_str_array(directories);
	if (result == PATH_ERROR)
		errno = ENOMEM;
	return (result);
}
