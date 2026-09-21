/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ryker_ft_file_unique.h                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 04:33:13 by hnah              #+#    #+#             */
/*   Updated: 2026/09/18 04:33:13 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RYKER_FT_FILE_UNIQUE_H
# define RYKER_FT_FILE_UNIQUE_H

# include "../../0_libft/libft.h"
# include <errno.h>
# include <fcntl.h>
# include <limits.h>

int	ryker_ft_create_open_unique_file(char **new_filepath,
		const char *prefix_filepath);

#endif
