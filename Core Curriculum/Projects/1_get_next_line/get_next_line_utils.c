/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 13:19:21 by hnah              #+#    #+#             */
/*   Updated: 2026/01/23 18:33:10 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int	check_char(char *new_stash)
{
	if (*new_stash == '\0')
		return (-2);
	while (*new_stash != '\0')
	{
		if (*new_stash == '\n')
			return (1);
		if (*(new_stash + 1) == '\0')
			return (0);
		new_stash++;
	}
	return (-1);
}

char	*gnl_strjoin_ret(char **buf, char **new_buf, ssize_t read_len)
{
	char	*imposter_buf;
	char	*gnl_buf;
	ssize_t	suffix;
	ssize_t	char_pos;

	suffix = read_len - find_len(new_buf, '\n');
	imposter_buf = malloc(sizeof(char) * (suffix + 1));
	char_pos = find_len(buf, '\0') + read_len - suffix;
	if (!imposter_buf)
		return (NULL);
	imposter_buf[suffix] = '\0';
	while (suffix--)
		imposter_buf[suffix] = new_buf[find_len(buf, '\0') + \
read_len - suffix];
	free(buf);
	buf = imposter_buf;
	gnl_buf = malloc(sizeof(char) * (char_pos + 1));
	if (!gnl_buf)
		return (NULL);
	gnl_buf[char_pos] = '\0';
	while (char_pos--)
		gnl_buf[char_pos] = new_buf[char_pos];
	free(new_buf);
	return (gnl_buf);
}

ssize_t	find_len(const char *s, const char *look_for)
{
	size_t	len;

	len = 0;
	while (*s && *s != *look_for)
	{
		len++;
		s++;
	}
	if (*s == * look_for)
		len++;
	return (len);
}