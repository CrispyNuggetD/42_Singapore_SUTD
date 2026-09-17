/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 21:05:36 by hnah              #+#    #+#             */
/*   Updated: 2026/09/17 22:27:55 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"
 
int	main(int argc, char **argv, char **envp)
{
	t_pipeline	pipeline;
	int			heredoc_exists;

	if (argc < 5)
		return (print_usage(USAGE_BONUS));
	heredoc_exists = is_exactly_heredoc(argv[1]);
	if (heredoc_exists && argc < 6)
		return (print_usage(USAGE_HEREDOC));
	init_pipeline(&pipeline);
	if (heredoc_exists && open_files_with_heredoc(&pipeline, argc, argv) != 0)
		return (close_pipeline(&pipeline));
	else if (!heredoc_exists && open_files_no_heredoc(&pipeline, argc, argv) != 0)
		return (close_pipeline(&pipeline));
	return (run_pipeline(&pipeline, argv, envp));
}
