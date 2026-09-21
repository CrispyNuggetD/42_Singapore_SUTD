/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_checker.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 19:10:52 by hnah              #+#    #+#             */
/*   Updated: 2026/09/21 16:45:12 by hnah             ###   ########.fr       */
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
		char **envp, char **exec_fail_path)
{
	char	*candidate_command;
	int		i;

	i = 0;
	while (directories[i])
	{
		candidate_command = join_command(directories[i++], args[0]);
		if (!candidate_command)
		{
			free(*exec_fail_path);
			*exec_fail_path = NULL;
			return (ALLOCATION_FAILED);
		}
		if (attempt_one_candidate(candidate_command, args,
				envp, exec_fail_path) == EXEC_FAILED)
			return (EXEC_FAILED);
	}
	if (!*exec_fail_path)
		return (COMMAND_NOT_FOUND);
	errno = EACCES;
	return (EXEC_FAILED);
}

static t_path_result	copy_direct_path(char *command, char **exec_fail_path)
{
	*exec_fail_path = ft_strdup(command);
	if (!*exec_fail_path)
	{
		errno = ENOMEM;
		return (ALLOCATION_FAILED);
	}
	return (DIRECT_PATH_SUPPLIED);
}

t_path_result	attempt_possible_candidates(char **args, char **envp,
		char **exec_fail_path)
{
	char			**directories;
	char			*path;
	t_path_result	result;

	*exec_fail_path = NULL;
	if (ft_strchr(args[0], '/'))
		return (copy_direct_path(args[0], exec_fail_path));
	path = path_value(envp);
	if (!path)
		path = "";
	directories = split_path_empty_as_dot(path);
	if (!directories)
	{
		errno = ENOMEM;
		return (ALLOCATION_FAILED);
	}
	result = search_directories(directories, args, envp, exec_fail_path);
	ryker_ft_free_str_array(directories);
	if (result == ALLOCATION_FAILED)
		errno = ENOMEM;
	return (result);
}
