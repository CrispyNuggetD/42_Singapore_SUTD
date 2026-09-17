/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ryker_ft_get_next_line.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 01:50:04 by hnah              #+#    #+#             */
/*   Updated: 2026/09/18 02:21:29 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ryker_ft_get_next_line.h"

void	ryker_ft_gnl_init(t_gnl_info *gnl, int fd)
{
	gnl->stash = NULL;
	gnl->fd = fd;
	gnl->eof = 0;
}

void	ryker_ft_gnl_cleanup(t_gnl_info *gnl)
{
	free(gnl->stash);
	gnl->stash = NULL;
	gnl->eof = 1;
}

static int	read_chunk(t_gnl_info *gnl)
{
	char		*new_stash;
	ssize_t		read_num;

	new_stash = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!new_stash)
		return (-1);
	read_num = read(gnl->fd, new_stash, BUFFER_SIZE);
	if (read_num < 0)
	{
		free(new_stash);
		return (-1);
	}
	new_stash[read_num] = '\0';
	gnl->eof = (read_num == 0);
	gnl->stash = gnl_strjoin(gnl->stash, new_stash);
	if (!gnl->stash)
		return (-1);
	return (0);
}

static t_gnl_result	take_line(t_gnl_info *gnl, char **line)
{
	if (!gnl->stash || gnl->stash[0] == '\0')
	{
		ryker_ft_gnl_cleanup(gnl);
		return (GNL_EOF);
	}
	*line = newline_ret(&gnl->stash);
	if (!*line)
	{
		ryker_ft_gnl_cleanup(gnl);
		return (GNL_ERROR);
	}
	return (GNL_LINE);
}

t_gnl_result	ryker_ft_get_next_line(t_gnl_info *gnl, char **line)
{
	ssize_t	nl;

	*line = NULL;
	if (gnl->fd < 0 || BUFFER_SIZE <= 0 || BUFFER_SIZE > SIZE_MAX - 1)
	{
		ryker_ft_gnl_cleanup(gnl);
		errno = EINVAL;
		return (GNL_ERROR);
	}
	while (!gnl->eof)
	{
		nl = find_len(gnl->stash, '\n');
		if (nl > 0 && gnl->stash[nl - 1] == '\n')
			break ;
		if (read_chunk(gnl) < 0)
		{
			ryker_ft_gnl_cleanup(gnl);
			return (GNL_ERROR);
		}
	}
	return (take_line(gnl, line));
}
