/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 20:23:22 by hnah              #+#    #+#             */
/*   Updated: 2026/01/15 21:12:30 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
	
char  *get_next_line(int fd)
{
	ssize_t		read_num;
		written;
	char		*placeholder;
	char		placeholder2;
	static char	*stash;
	
	i = 0;
	read_num = 0;
	if (fd < 0 || BUFFER_SIZE <= 0 || SIZE_MAX - BUFFER_SIZE < 0)
		return (NULL);
	read_num = read(fd, CURRENTSTASHPOINTERTHATCANTAKE THE BUFFER SIZE, BUFFER_SIZE);
	if (function_placeholder(input) < 0)
		return (something of type char* that means something);
	return (the nex));
}

/*
SEARCH "IMPORTANT"
cases = 
1. read returns < buffer size MEANS EOF reached or STDIN < buffsize\
2. read returns == buffer size
	2.1 a) function 1 (Check read if contains important info)
	1. \n is present
		1.1 possible ways:
		1.1.1 last character
			2.1.1 \n = newline encountered at this very end of read instance where buffersize is actually apparently filled and last is get next line
				2.1.1.1 Implications : 
					A) IMPORTANT STASH CAN BE EMPTY AFTER CLEARANCE
		1.1.2 first
			A) CANNOT DEREF BEFORE AN ARRAY ALWAYS DANGEROUS
		1.1.3 mid
		b) send to printer if GNL'd
			c) clear and reset stash after printer with the current situation of the stash:
				1. 


*/

/*
possible situations: nothing in file, file only new linees and empty, file has a line as long as buffer (need to guard NULL wrongly, do i need to? and if i need to, how?), and the situation of reaching the end of file directly (buff = empty) as well as buffer containing EOF (what's EOF in read? or is it just '\0' or that buffer is empty/ null?)) so first get buffer, while not rewline character (or null?), static append.
*/