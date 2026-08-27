/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 19:10:52 by hnah              #+#    #+#             */
/*   Updated: 2026/08/27 19:22:31 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	exit_perror(char *message, int status)
{
	perror(message);
	exit(status);
}

static void	command_not_found(char *command, char **args)
{
	write(2, "pipex: ", 7);
	write(2, command, ft_strlen(command));
	write(2, ": command not found\n", 20);
	free_matrix(args);
	exit(127);
}

void	execute_command(char *command, char **envp)
{
	char	**args;
	char	*path;

	args = ft_split(command, ' ');
	if (!args)
		exit_perror("malloc", 1);
	if (!args[0])
	{
		free_matrix(args);
		write(2, "pipex: command not found\n", 25);
		exit(127);
	}
	path = resolve_path(args[0], envp);
	if (!path)
		command_not_found(args[0], args);
	execve(path, args, envp);
	perror(args[0]);
	free(path);
	free_matrix(args);
	exit(126);
}
