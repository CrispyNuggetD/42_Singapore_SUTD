/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 13:19:21 by hnah              #+#    #+#             */
/*   Updated: 2026/01/20 22:35:04 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int	check_char(char *new_stash)
{
	while (*new_stash != '\0')
	{
		if (*new_stash == '\n')
			return (1);
		if (*(new_stash + 1) == '\0')
			return (0);
	}
	return (0);
}

char	*gnl_strjoin(char *buf, char *new_buf, ssize_t read_len)
{
	char	*imposter_buf;
	char	*gnl_buf;
	ssize_t	remainder;
	ssize_t	char_pos;

	remainder = read_len - find(new_buf, '\n');
	imposter_buf = malloc(sizeof(char) * (remainder + 1));
	char_pos = find(buf, NULL) + read_len - remainder;
	if (!imposter_buf)
		return (NULL);
	imposter_buf[remainder] = '\0';
	while (remainder--)
		imposter_buf[remainder] = new_buf[find(buf, NULL) + \
read_len - remainder];
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

ssize_t	find(const char *s, const char *look_for)
{
	size_t	len;

	len = 0;
	while (*s && *s != look_for)
	{
		len++;
		s++;
	}
	if (s == look_for)
		len++;
	return (len);
}