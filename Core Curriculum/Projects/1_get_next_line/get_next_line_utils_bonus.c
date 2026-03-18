/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils_bonus.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 13:19:21 by hnah              #+#    #+#             */
/*   Updated: 2026/03/06 18:54:18 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

ssize_t	find_len(const char *s, int look_for)
{
	ssize_t	len;

	len = 0;
	if (!s)
		return (0);
	while (*s && *s != look_for)
	{
		len++;
		s++;
	}
	if (*s && *s == look_for)
		len++;
	return (len);
}

char	*newline_ret(char **buf)
{
	char	*temp_buf;
	char	*gnl_buf;
	ssize_t	prefix;
	ssize_t	suffix;

	prefix = find_len(*buf, '\n');
	suffix = find_len(*buf, '\0') - prefix;
	temp_buf = malloc(sizeof(char) * (suffix + 1));
	if (!temp_buf)
		return (NULL);
	temp_buf[suffix] = '\0';
	while (suffix--)
		temp_buf[suffix] = (*buf)[prefix + suffix];
	gnl_buf = malloc(sizeof(char) * (prefix + 1));
	if (!gnl_buf)
	{
		free(temp_buf);
		return (NULL);
	}
	gnl_buf[prefix] = '\0';
	while (prefix--)
		gnl_buf[prefix] = (*buf)[prefix];
	free(*buf);
	*buf = temp_buf;
	return (gnl_buf);
}

char	*read_error(char **stash, char *new_stash)
{
	free(new_stash);
	free(*stash);
	*stash = NULL;
	return (NULL);
}

char	*gnl_strjoin(char *buf, char *new_buf)
{
	char	*temp_buf;
	ssize_t	new_buf_len;
	ssize_t	buf_len;

	new_buf_len = find_len(new_buf, '\0');
	buf_len = find_len(buf, '\0');
	temp_buf = malloc(sizeof(char) * (new_buf_len + buf_len + 1));
	if (!temp_buf)
	{
		free(buf);
		free(new_buf);
		return (NULL);
	}
	temp_buf[new_buf_len + buf_len] = '\0';
	while (new_buf_len--)
		temp_buf[buf_len + new_buf_len] = new_buf[new_buf_len];
	while (buf_len--)
		temp_buf[buf_len] = buf[buf_len];
	free(buf);
	free(new_buf);
	return (temp_buf);
}

char	*fd_end_handler(char **stash)
{
	if (*stash && (*stash)[0] != '\0')
		return (newline_ret(stash));
	free(*stash);
	*stash = NULL;
	return (NULL);
}
