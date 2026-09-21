/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_candidate.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 15:07:34 by hnah              #+#    #+#             */
/*   Updated: 2026/09/21 16:07:03 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

/* Success never returns; otherwise continue or report a final failure. */
t_path_result	attempt_one_candidate(char *candidate_command, char **args,
		char **envp, char **exec_fail_path)
{
	int	saved_errno;

	if (access(candidate_command, F_OK) == 0)
	{
		execve(candidate_command, args, envp);
		saved_errno = errno;
		if (saved_errno != EACCES)
		{
			free(*exec_fail_path);
			*exec_fail_path = candidate_command;
			errno = saved_errno;
			return (EXEC_FAILED);
		}
	}
	if (errno == EACCES && !*exec_fail_path)
	{
		*exec_fail_path = candidate_command;
		return (SEARCH_CONTINUE);
	}
	free(candidate_command);
	return (SEARCH_CONTINUE);
}
