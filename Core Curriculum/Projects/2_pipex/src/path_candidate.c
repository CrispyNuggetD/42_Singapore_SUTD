/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_candidate.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 19:10:52 by hnah              #+#    #+#             */
/*   Updated: 2026/09/17 22:14:36 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

/* Success never returns; otherwise continue or report a final failure. */
t_path_result	attempt_one_candidate(char *candidate_command, char **args,
		char **envp, char **command_path)
{
	int	saved_errno;

	if (access(candidate_command, F_OK) == 0)
	{
		execve(candidate_command, args, envp);
		saved_errno = errno;
		if (saved_errno != EACCES)
		{
			free(*command_path);
			*command_path = candidate_command;
			errno = saved_errno;
			return (EXEC_FAILED);
		}
		if (!*command_path)
		{
			*command_path = candidate_command;
			return (SEARCH_CONTINUE);
		}
	}
	free(candidate_command);
	return (SEARCH_CONTINUE);
}
