/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 20:23:22 by hnah              #+#    #+#             */
/*   Updated: 2026/02/09 18:12:23 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
	
char  *get_next_line(int fd)
{
	static char	*stash;
	char		*new_stash;
	char		*ret_line;
	ssize_t		read_num;
	
	if (fd < 0 || fd > 1024 || BUFFER_SIZE <= 0 || SIZE_MAX - BUFFER_SIZE < 0)
		return (NULL);
	while (1)
	{
		if (find_len(stash, '\0') != find_len(stash, '\n'))
			return (newline_ret(&stash));
		new_stash = malloc(sizeof(char) * (BUFFER_SIZE + 1));
		if (!new_stash)
			return (NULL);
		read_num = read(fd, new_stash, BUFFER_SIZE);
		if (read_num < 0)
		{
			if (stash)
				free(stash);
			if (new_stash)
				free(new_stash);
			return (NULL);
		}
		??? = gnl_strjoin(stash, new_stash);
		if (read_num == 0)
		{
			if (find_len(stash, '\0') != find_len(stash, '\n'))
				return (newline_ret(&stash));
			ret_line = stash;
			stash = NULL;
			return (ret_line);
				
			return (gnl_strjoin_ret(&stash, &new_stash, read_num));
		
			if (stash)
				return (stash);
			free(stash);
			free(new_stash);
			return (NULL);
		} 
	}
} 


/*
SEARCH "IMPORTANT"
cases = 
1. read returns < buffer size MEANS EOF reached or STDIN < buffsize\
2. read returns == buffer size
3 a) function 1 (Check read if contains important info)
	1. \n is present
		1.1 possible ways:
		1.1.1 last character
			2.1.1 \n = newline encountered at this very end of read instance where buffersize is actually apparently filled and last is get next line
				2.1.1.1 Implications : 
					A) IMPORTANT STASH CAN BE EMPTY AFTER CLEARANCE
		1.1.2 first
					B) CANNOT DEREF BEFORE AN ARRAY ALWAYS DANGEROUS
		1.1.3 mid
					C) need to rmb function that "clears stash" needs to push back original content after \n to stash too
					
		b) send to printer if GNL'd
		c) clear and reset stash after printer with the current situation of the stash:
			1. Nothing: Reset to NULL again
			2. SOmething:


*/

/*
possible situations: nothing in file, file only new linees and empty, file has a line as long as buffer (need to guard NULL wrongly, do i need to? and if i need to, how?), and the situation of reaching the end of file directly (buff = empty) as well as buffer containing EOF (what's EOF in read? or is it just '\0' or that buffer is empty/ null?)) so first get buffer, while not rewline character (or null?), static append.
*/