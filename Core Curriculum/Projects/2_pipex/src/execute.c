/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 19:10:52 by hnah              #+#    #+#             */
/*   Updated: 2026/09/17 22:17:20 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static void	command_not_found(char *command, char **args)
{
	ryker_ft_printf_fd(2, "pipex: %s: command not found\n", command);
	ryker_ft_free_str_array(args);
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
		ryker_ft_free_str_array(args);
		ryker_ft_printf_fd(2, "pipex: command not found\n");
		exit(127);
	}
	path = resolve_path(args[0], envp);
	if (!path)
		command_not_found(args[0], args);
	execve(path, args, envp);
	perror(args[0]);
	free(path);
	ryker_ft_free_str_array(args);
	exit(126);
}
