/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 20:23:22 by hnah              #+#    #+#             */
/*   Updated: 2026/03/10 17:34:02 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*get_next_line(int fd)
{
	static char	*stash;
	char		*new_stash;	
	ssize_t		read_num;
	ssize_t		nl;

	if (fd < 0 || fd > 1024 || BUFFER_SIZE <= 0 || BUFFER_SIZE > SIZE_MAX - 1)
		return (NULL);
	while (1)
	{
		nl = find_len(stash, '\n');
		if (nl > 0 && stash[nl - 1] == '\n')
			return (newline_ret(&stash));
		new_stash = malloc(sizeof(char) * (BUFFER_SIZE + 1));
		if (!new_stash)
			return (NULL);
		read_num = read(fd, new_stash, BUFFER_SIZE);
		if (read_num < 0)
			return (read_error(&stash, new_stash));
		new_stash[read_num] = '\0';
		stash = gnl_strjoin(stash, new_stash);
		if (!stash)
			return (NULL);
		if (read_num == 0)
			return (fd_end_handler(&stash));
	}
}
