/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 19:10:52 by hnah              #+#    #+#             */
/*   Updated: 2026/09/18 22:54:04 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static void	command_not_found(char *command, char **args)
{
	if (command)
		ryker_ft_printf_fd(2, "pipex: %s: command not found\n", command);
	else
		ryker_ft_printf_fd(2, "pipex: command not found\n");
	ryker_ft_free_str_array(args);
	exit(127);
}

static void	path_lookup_error(char **args)
{
	perror("pipex: resolve_path");
	ryker_ft_free_str_array(args);
	exit(1);
}

static int	exec_failure_status(char *path, int saved_errno)
{
	if (saved_errno != ENOENT)
		return (126);
	if (access(path, F_OK) == 0)
		return (126);
	if (errno == ENOENT)
		return (127);
	return (126);
}

static void	exec_failure(char *path, char **args, int saved_errno)
{
	int	status;

	status = exec_failure_status(path, saved_errno);
	errno = saved_errno;
	perror(args[0]);
	free(path);
	ryker_ft_free_str_array(args);
	exit(status);
}

void	execute_command(char *command, char **envp)
{
	char			**args;
	char			*path;
	t_path_result	result;

	args = ft_split(command, ' ');
	if (!args)
		exit_perror("malloc", 1);
	if (!args[0])
		command_not_found(NULL, args);
	result = resolve_path(args[0], envp, &path);
	if (result == PATH_ERROR)
		path_lookup_error(args);
	if (result == PATH_NOT_FOUND)
		command_not_found(args[0], args);
	execve(path, args, envp);
	exec_failure(path, args, errno);
}
