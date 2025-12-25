/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putendl_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/14 14:58:53 by hnah              #+#    #+#             */
/*   Updated: 2025/12/16 12:10:31 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putendl_fd(char *s, int fd)
{
	char	nl;
	size_t	len;

	nl = '\n';
	len = 0;
	if (fd < 0)
		return ;
	while (s[len])
		len++;
	write(fd, s, len);
	write(fd, &nl, 1);
	return ;
}
