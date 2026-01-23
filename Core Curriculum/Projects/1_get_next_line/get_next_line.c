/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 20:23:22 by hnah              #+#    #+#             */
/*   Updated: 2026/01/23 18:33:08 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
	
char  *get_next_line(int fd)
{
	static char	*stash;
	char		*new_stash;
	char		*last_stash;// ?????
	int			is_event;
	ssize_t		read_num;
	ssize_t		stash_len;
	
	if (fd < 0 || fd > 1024 || BUFFER_SIZE <= 0 || SIZE_MAX - BUFFER_SIZE < 0)
		return (NULL);
	while (1)
	{
		if (find_len(stash, '\0') != find_len(stash, '\n'))
			return (gnl_strjoin_ret(&stash, &new_stash, read_num));
		stash_len = find_len(stash, '\0');
		new_stash = malloc(sizeof(char) * (stash_len + BUFFER_SIZE + 1));
		if (!new_stash)
			return (NULL);
		read_num = read(fd, new_stash + stash_len, BUFFER_SIZE);
		if (read_num < 0)
			if (stash)
				free(stash);
			return (NULL);
		if (read_num == 0)
			if (stash)
				
				free(stash);
		new_stash[stash_len + read_num] = '\0';
		while (stash_len--)
			new_stash[stash_len] = stash[stash_len];
		is_event = check_char(new_stash);
		if (is_event >= 0)
			return (gnl_strjoin_ret(&stash, &new_stash, read_num));
		else if (is_event == -1) //is_event is -1, continue
			gnl_strjoin_ret(&stash, &new_stash, read_num);
		else if (is_event == -2)
		{
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