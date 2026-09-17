/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 19:05:09 by hnah              #+#    #+#             */
/*   Updated: 2026/09/17 17:53:54 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	main(int argc, char **argv, char **envp)
{
	t_pipeline	pipeline;
	int			status;

	if (argc != 5)
		return (print_usage(USAGE_MANDATORY));
	init_pipeline(&pipeline);
	if (open_files_no_heredoc(&pipeline, argc, argv) != 0)
		return (close_pipeline(&pipeline));
	status = spawn_children(&pipeline, argv, envp);
			//pipeline_close(&pipeline);
	return (status);
}
