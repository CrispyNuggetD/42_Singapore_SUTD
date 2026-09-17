/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ryker_ft_create_open_unique_file.c                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 04:33:13 by hnah              #+#    #+#             */
/*   Updated: 2026/09/18 04:49:03 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ryker_ft_file_unique.h"

static char	*append_numeric_suffix(const char *prefix, int number)
{
	char	*digits;
	char	*filepath;

	digits = ft_itoa(number);
	if (!digits)
		return (NULL);
	filepath = ft_strjoin(prefix, digits);
	free(digits);
	return (filepath);
}

static int	file_error(char **new_filepath, int original_error)
{
	if (new_filepath)
	{
		free(*new_filepath);
		*new_filepath = NULL;
	}
	errno = original_error;
	return (-1);
}

static int	open_candidate(char **new_filepath, const char *prefix, int number)
{
	int	fd;

	*new_filepath = append_numeric_suffix(prefix, number);
	if (!*new_filepath)
		return (file_error(new_filepath, ENOMEM));
	fd = open(*new_filepath, O_WRONLY | O_CREAT | O_EXCL, 0600);
	if (fd < 0)
		return (file_error(new_filepath, errno));
	return (fd);
}

int	ryker_ft_create_open_unique_file(char **new_filepath,
		const char *prefix_filepath)
{
	int	number;
	int	fd;

	if (!new_filepath)
		return (file_error(NULL, EINVAL));
	*new_filepath = NULL;
	if (!prefix_filepath || !*prefix_filepath)
		return (file_error(new_filepath, EINVAL));
	number = 0;
	while (number < INT_MAX)
	{
		fd = open_candidate(new_filepath, prefix_filepath, number);
		if (fd >= 0 || errno != EEXIST)
			return (fd);
		number++;
	}
	return (file_error(new_filepath, EEXIST));
}
