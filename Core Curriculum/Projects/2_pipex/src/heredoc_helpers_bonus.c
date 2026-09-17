/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_helpers_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 13:57:17 by hnah              #+#    #+#             */
/*   Updated: 2026/09/17 23:04:19 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

int	is_exactly_heredoc(char *argument)
{
	return (ft_strncmp(argument, "here_doc", sizeof("here_doc")) == 0);
}

int	is_limiter_bonus(char *line, char *limiter)
{
	size_t	length;

	length = ft_strlen(limiter);
	if (ft_strncmp(line, limiter, length) != 0)
		return (0);
	if (line[length] == '\0')
		return (1);
	if (line[length] == '\n' && line[length + 1] == '\0')
		return (1);
	return (0);
}

int	write_line_bonus(int fd, char *line)
{
	if (ryker_ft_printf_fd(fd, "%s", line) < 0)
		return (-1);
	return (0);
}
