/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_helpers_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 13:57:17 by hnah              #+#    #+#             */
/*   Updated: 2026/09/17 17:04:42 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	is_heredoc(char *argument)
{
	return (ft_strncmp(argument, "here_doc", 9) == 0);
}

int	open_files_with_heredoc(t_pipeline *pipeline, int argc, char **argv)
{
	pipeline->first_command = 3;
	pipeline->last_command = argc - 2;
	pipeline->input_fd = prepare_heredoc_bonus(argv[2]);
	if (pipeline->input_fd < 0)
		return (1);
	pipeline->output_fd = open_output(argv[argc - 1], O_APPEND);
	if (pipeline->output_fd < 0)
		return (return_perror(argv[argc - 1]));
	return (0);
}