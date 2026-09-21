/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_checker.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 19:10:52 by hnah              #+#    #+#             */
/*   Updated: 2026/09/21 14:49:51 by hnah             ###   ########.fr       */
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

static t_path_result	search_directories(char **directories, char **args,
		char **envp, char **command_path)
{
	char	*candidate_command;
	int		i;

	i = 0;
	while (directories[i])
	{
		candidate_command = join_command(directories[i++], args[0]);
		if (!candidate_command)
		{
			free(*command_path);
			*command_path = NULL;
			return (PATH_ERROR);
		}
		if (attempt_one_candidate(candidate_command, args,
				envp, command_path) == EXEC_FAILED)
			return (EXEC_FAILED);
	}
	if (!*command_path)
		return (PATH_NOT_FOUND);
	errno = EACCES;
	return (EXEC_FAILED);
}

static t_path_result	copy_direct_path(char *command, char **command_path)
{
	*command_path = ft_strdup(command);
	if (!*command_path)
	{
		errno = ENOMEM;
		return (PATH_ERROR);
	}
	return (DIRECT_PATH_SUPPLIED);
}

t_path_result	attempt_possible_candidates(char **args, char **envp,
		char **command_path)
{
	char			**directories;
	char			*path;
	t_path_result	result;

	*command_path = NULL;
	if (ft_strchr(args[0], '/'))
		return (copy_direct_path(args[0], command_path));
	path = path_value(envp);
	if (!path)
		return (PATH_NOT_FOUND);
	directories = ft_split(path, ':');
	if (!directories)
	{
		errno = ENOMEM;
		return (PATH_ERROR);
	}
	result = search_directories(directories, args, envp, command_path);
	ryker_ft_free_str_array(directories);
	if (result == PATH_ERROR)
		errno = ENOMEM;
	return (result);
}
