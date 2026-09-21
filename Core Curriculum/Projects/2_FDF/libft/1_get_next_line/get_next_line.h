/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 13:22:29 by hnah              #+#    #+#             */
/*   Updated: 2026/03/05 18:06:07 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 67
# endif

# include <unistd.h>
# include <stdlib.h>
# include <limits.h>
# include <stdint.h>

char	*get_next_line(int fd);
char	*newline_ret(char **buf);
char	*read_error(char **stash, char *new_stash);
char	*gnl_strjoin(char *buf, char *new_buf);
ssize_t	find_len(const char *s, int look_for);
char	*fd_end_handler(char **stash);

#endif
