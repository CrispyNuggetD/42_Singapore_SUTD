/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd_open_utils_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 22:26:21 by hnah              #+#    #+#             */
/*   Updated: 2026/09/17 22:26:21 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

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
