/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 21:05:36 by hnah              #+#    #+#             */
/*   Updated: 2026/09/18 03:27:14 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_BONUS_H
# define PIPEX_BONUS_H

# include "pipex.h"

int		is_exactly_heredoc(char *argument);
int		open_files_with_heredoc(t_pipeline *pipeline, int argc, char **argv);
int		is_limiter(char *line, char *limiter);
int		write_line_guaranteed(int heredoc_write_fd, char *line);
int		prepare_heredoc(char *limiter);

int		heredoc_cleanup(t_gnl_info *gnl, char *line, int status);

#endif
