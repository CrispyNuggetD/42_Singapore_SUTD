/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ryker_ft_get_next_line.h                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 01:50:04 by hnah              #+#    #+#             */
/*   Updated: 2026/09/18 02:21:29 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RYKER_FT_GET_NEXT_LINE_H
# define RYKER_FT_GET_NEXT_LINE_H

# include "../../1_get_next_line/get_next_line.h"
# include <errno.h>

typedef enum e_gnl_result
{
	GNL_ERROR = -1,
	GNL_EOF = 0,
	GNL_LINE = 1
}	t_gnl_result;

typedef struct s_gnl_info
{
	char	*stash;
	int		fd;
	int		eof;
}	t_gnl_info;

void			ryker_ft_gnl_init(t_gnl_info *gnl, int fd);
t_gnl_result	ryker_ft_get_next_line(t_gnl_info *gnl, char **line);
void			ryker_ft_gnl_cleanup(t_gnl_info *gnl);

#endif
