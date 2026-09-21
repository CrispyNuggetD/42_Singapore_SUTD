/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ryker_ft_create_open_unique_file_in_dir.c          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 04:33:13 by hnah              #+#    #+#             */
/*   Updated: 2026/09/18 05:02:06 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ryker_ft_file_unique.h"
#include <sys/stat.h>

/*
** WIP: experimental code retained for study, not an approved public API.
** I have not yet reviewed and understood this implementation sufficiently
** to take ownership of it. Passing tests does not make it ready for use.
** Excluded from normal builds; do not copy this source into Pipex.
*/

static int	ensure_directory(const char *directory)
{
	struct stat	info;

	if (mkdir(directory, 0700) == 0)
		return (0);
	if (errno != EEXIST)
		return (-1);
	if (stat(directory, &info) < 0)
		return (-1);
	if (!S_ISDIR(info.st_mode))
	{
		errno = ENOTDIR;
		return (-1);
	}
	return (0);
}

static char	*join_prefix(const char *directory, const char *prefix)
{
	char	*with_slash;
	char	*filepath;

	if (directory[ft_strlen(directory) - 1] == '/')
		return (ft_strjoin(directory, prefix));
	with_slash = ft_strjoin(directory, "/");
	if (!with_slash)
		return (NULL);
	filepath = ft_strjoin(with_slash, prefix);
	free(with_slash);
	return (filepath);
}

static int	create_in_directory(char **new_filepath, const char *directory,
		const char *prefix_filepath)
{
	if (ensure_directory(directory) < 0)
		return (-1);
	return (ryker_ft_create_open_unique_file(new_filepath, prefix_filepath));
}

int	ryker_ft_create_open_unique_file_in_dir(char **new_filepath,
		const char *directory, const char *prefix)
{
	char	*prefix_filepath;
	int		fd;
	int		error;

	if (new_filepath)
		*new_filepath = NULL;
	if (!new_filepath || !directory || !*directory || !prefix || !*prefix
		|| ft_strchr(prefix, '/'))
	{
		errno = EINVAL;
		return (-1);
	}
	prefix_filepath = join_prefix(directory, prefix);
	if (!prefix_filepath)
	{
		errno = ENOMEM;
		return (-1);
	}
	fd = create_in_directory(new_filepath, directory, prefix_filepath);
	error = errno;
	free(prefix_filepath);
	errno = error;
	return (fd);
}
