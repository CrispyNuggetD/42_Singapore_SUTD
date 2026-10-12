/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   write_guaranteed.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/10/12 19:37:39 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** Adapted from my Push_swap ft_printf helper, without its print context.
** Finish partial writes; report failure if write errors or makes no progress.
*/
t_status	write_guaranteed(int fd, const char *buffer, size_t length)
{
	ssize_t	written;

	while (length > 0)
	{
		written = write(fd, buffer, length);
		if (written <= 0)
			return (ERR_WRITE);
		buffer += written;
		length -= (size_t)written;
	}
	return (STATUS_OK);
}
