/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 21:05:36 by hnah              #+#    #+#             */
/*   Updated: 2026/09/17 22:27:55 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_BONUS_H
# define PIPEX_BONUS_H

# include "pipex.h"

int		is_exactly_heredoc(char *argument);
int		open_files_with_heredoc(t_pipeline *pipeline, int argc, char **argv);
int		read_line_bonus(int fd, char **line);
int		is_limiter_bonus(char *line, char *limiter);
int		write_line_bonus(int fd, char *line);
int		prepare_heredoc_bonus(char *limiter);

#endif
