/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 21:05:36 by hnah              #+#    #+#             */
/*   Updated: 2026/09/11 21:05:36 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

static void	init_pipeline(t_pipeline *pipeline)
{
	pipeline->input_fd = -1;
	pipeline->output_fd = -1;
	pipeline->pipefd[0] = -1;
	pipeline->pipefd[1] = -1;
	pipeline->child_count = 0;
	pipeline->last_pid = -1;
}

static int	is_heredoc(char *argument)
{
	return (ft_strncmp(argument, "here_doc", 9) == 0);
}

static int	usage_bonus(int heredoc)
{
	if (heredoc)
		ft_printf_fd(2, "Usage: ./pipex here_doc LIMITER cmd... outfile\n");
	else
		ft_printf_fd(2, "Usage: ./pipex infile cmd... outfile\n");
	return (1);
}

static int	open_files(t_pipeline *pipeline, int argc, char **argv,
		int heredoc)
{
	if (heredoc)
	{
		pipeline->input_fd = prepare_heredoc_bonus(argv[2]);
		pipeline->first_command = 3;
		if (pipeline->input_fd < 0)
			return (1);
	}
	else
	{
		pipeline->input_fd = open(argv[1], O_RDONLY);
		pipeline->first_command = 2;
		if (pipeline->input_fd < 0)
			perror(argv[1]);
	}
	pipeline->last_command = argc - 2;
	if (heredoc)
		pipeline->output_fd = open(argv[argc - 1],
				O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		pipeline->output_fd = open(argv[argc - 1],
				O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (pipeline->output_fd < 0)
		return (return_perror(argv[argc - 1]));
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	t_pipeline	pipeline;
	int			heredoc;

	if (argc < 5)
		return (usage_bonus(0));
	heredoc = is_heredoc(argv[1]);
	if (heredoc && argc < 6)
		return (usage_bonus(1));
	init_pipeline(&pipeline);
	if (open_files(&pipeline, argc, argv, heredoc) != 0)
	{
		if (pipeline.input_fd >= 0)
			close(pipeline.input_fd);
		return (1);
	}
	return (run_pipeline_bonus(&pipeline, argv, envp));
}
