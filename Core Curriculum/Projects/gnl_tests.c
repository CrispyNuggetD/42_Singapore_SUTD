/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gnl_tests.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 14:13:01 by thtay             #+#    #+#             */
/*   Updated: 2026/03/02 16:55:29 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>

char	*ft_strdup(const char *s)
{
	size_t	i;
	char	*new;

	i = 0;
	while (s[i])
		i++;
	new = malloc(i + 1);
	if (!new)
		return (NULL);
	i = 0;
	while (s[i])
	{
		new[i] = s[i];
		i++;
	}
	new[i] = '\0';
	return (new);
}

void readdoc(void)
{
	int	i;

	i = 0;
	while (1)
	{
		char *prt = get_next_line(1);
		if (prt == NULL)
			break ;
	 	printf ("Line %3i: %s\n", i, prt);
		free(prt);
		i++;
	}
}

int main(void)
{
	int i = 0;
	int fd = open("help1.txt", O_RDONLY, 777);
	int fd2 = open ("help2.txt", O_RDONLY, 777);
	char *prt = ft_strdup(""), *prt2 = ft_strdup("");
	while (1)
	{
		if (prt != NULL)
			free (prt);
		prt = get_next_line(fd);
		if (prt == NULL && prt2 == NULL)
			break ;
		printf ("FD%2iLine %3i: %s\n", fd, i, prt);

		if (prt2 != NULL)
			free (prt2);
		prt2 = get_next_line(fd2);
		if (prt == NULL && prt2 == NULL)
			break ;
		printf ("FD%2iLine %3i: %s\n", fd2, i, prt2);

		i++;
	}
	free(prt);
	free(prt2);
	close(fd);
	close(fd2); printf("\n\n");
	readdoc();
}
