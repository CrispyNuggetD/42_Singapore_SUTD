/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 14:33:46 by hnah              #+#    #+#             */
/*   Updated: 2025/12/11 15:54:45 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	char	*moving;
	char	*last_found;

	moving = (char *)s;
	last_found = NULL;
	while (*moving)
	{
		if (*moving == (char)c)
			last_found = moving;
		moving++;
	}
	if ((char)c == '\0')
		return (moving);
	return (last_found);
}

// last_found will be NULL anyway if doesn't exist
