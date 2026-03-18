/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 20:23:22 by hnah              #+#    #+#             */
/*   Updated: 2026/03/06 18:54:20 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

char	*get_next_line(int fd)
{
	static char	*stash[1024];
	char		*new_stash;	
	ssize_t		read_num;
	ssize_t		nl;

	if (fd < 0 || fd > 1024 || BUFFER_SIZE <= 0 || BUFFER_SIZE > SIZE_MAX - 1)
		return (NULL);
	while (1)
	{
		nl = find_len(stash[fd], '\n');
		if (nl > 0 && stash[fd][nl - 1] == '\n')
			return (newline_ret(&stash[fd]));
		new_stash = malloc(sizeof(char) * (BUFFER_SIZE + 1));
		if (!new_stash)
			return (NULL);
		read_num = read(fd, new_stash, BUFFER_SIZE);
		if (read_num < 0)
			return (read_error(&stash[fd], new_stash));
		new_stash[read_num] = '\0';
		stash[fd] = gnl_strjoin(stash[fd], new_stash);
		if (!stash[fd])
			return (NULL);
		if (read_num == 0)
			return (fd_end_handler(&stash[fd]));
	}
}
