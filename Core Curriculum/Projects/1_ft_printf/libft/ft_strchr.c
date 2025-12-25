/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 12:47:54 by hnah              #+#    #+#             */
/*   Updated: 2025/12/11 14:32:03 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	char	*moving;

	moving = (char *)s;
	while (*moving)
	{
		if (*moving == (char)c)
			return (moving);
		moving++;
	}
	if ((char)c == '\0')
		return (moving);
	return (NULL);
}
