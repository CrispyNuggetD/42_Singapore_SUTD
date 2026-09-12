/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 21:05:36 by hnah              #+#    #+#             */
/*   Updated: 2026/09/11 21:05:36 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

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
	if (ft_printf_fd(fd, "%s", line) < 0)
		return (-1);
	return (0);
}
