/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 19:10:52 by hnah              #+#    #+#             */
/*   Updated: 2026/09/21 14:49:58 by hnah             ###   ########.fr       */
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
	perror("pipex: attempt_possible_candidates");
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

static void	exec_failure_error(char *path, char **args, int saved_errno)
{
	int	status;

	status = exec_failure_status(path, saved_errno);
	errno = saved_errno;
	perror(path);
	free(path);
	ryker_ft_free_str_array(args);
	exit(status);
}

void	execute_command(char *command_str, char **envp)
{
	char			**args;
	char			*exec_fail_path;
	t_path_result	result;

	args = ft_split(command_str, ' ');
	if (!args)
		exit_perror("malloc", 1);
	if (!args[0])
		command_not_found(NULL, args);
	result = attempt_possible_candidates(args, envp, &exec_fail_path);
	if (result == ALLOCATION_FAILED)
		path_lookup_error(args);
	if (result == COMMAND_NOT_FOUND)
		command_not_found(args[0], args);
	if (result == DIRECT_PATH_SUPPLIED)
		execve(exec_fail_path, args, envp);
	exec_failure_error(exec_fail_path, args, errno);
}
