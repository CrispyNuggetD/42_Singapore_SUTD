/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 20:04:40 by hnah              #+#    #+#             */
/*   Updated: 2025/12/11 20:21:52 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *s)
{
	char	*d;
	char	*d_backup;
	char	*s_backup;
	int		s_len;

	s_len = 0;
	s_backup = s;	
	while (*s_backup)
		s_backup++;
	d = malloc(s_len + 1);
	if (!d)
		return (NULL);
	d_backup = d;
	while (s_len--)
		*d++ = *s++;
	*d = '\0';
	return (d_backup);
}

// Yes, it's simple coping so I did a:
// (s_len--) since this loops s_len times
// 	*d++ = *s++;
