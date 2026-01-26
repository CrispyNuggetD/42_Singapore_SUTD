/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 13:19:21 by hnah              #+#    #+#             */
/*   Updated: 2026/01/26 12:57:50 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int	check_char(char *new_stash)
{
	if (*new_stash == '\0')
		return (-2);
	while (*new_stash != '\0')
	{
		if (*new_stash == '\n')
			return (1);
		if (*(new_stash + 1) == '\0')
			return (0);
		new_stash++;
	}
	return (-1);
}

/*
*gnl_strjoin_ret always frees temporary stash used by overwriting before returning.
if i want to use for both, 2 cases, 1 continue (no /n) another is ys (\n)
Cases: GNL (if contains \n == TRUE, Immediately):
Case 1:
12\n45678\0x (len might be < BUFFER_SIZE + 1 but definitely \0)
Running(*gnl_strjoin_ret):

Else Cases...: 
Case 2,3: GNL continue (GOTO)

=====
B: Index (+1 if \n):
Case 1:
\n OR \0: 2+1 = 3
\0 8

Case 2:
\n OR \0: 5
\0 = 5
=====
Algo:
Case 1: GNL (if contains \n == TRUE, Immediately,):

=====
Running(*gnl_strjoin_ret):
Case 1: \n OR \0 at 2
(2+1)+1 = 4
3* cpy + \0 = 12\n\0 (new_buf overwrite)
-----
Case 1 continued: \0 at 8
remainder = 8 read - 3 = 5
5+1*cpy+\0 = 45678\0
(buf overwrite)
(new_buf return)
END case 1:
buf = 45678\0 (Case 2)
new_buf = 123\n\0
=====
=====
(FROM GOTO) Case 2,3: GNL (if contains \n == FALSE)

GNL (contains \n == FALSE):
Possible cases:
Buf at start of GNL: 
Case 2:
12345\0xxxxx
Case 3:
1234567890\0 (Or any longer buf)
Case 4:
\0

1. Make new_buf that can take len (till \0) buf + new read (BUFFER_SIZE+1)
2. Copy buf to new_buf up till BEFORE \0
3. Read to new_buf (offset by buf)
4. \0 the buf read location
5. Running(*gnl_strjoin_ret) (GOTO NEXT)
=====
Case 2:
12345\0xxxxxxxxxx
-----
Running(*gnl_strjoin_ret):
Case 2: \n OR \0 at 5
5*cpy + \0 = 12345\0 (new_buf overwrite)
-----
Case 2 continued: \0 at 5
remainder = 5 read - 5 = 0
01*cpy+\0 = 0\0
(buf overwrite)
(new_buf return)
END case 2:
buf = \0 (Case 2)
new_buf = 12345\0
=====
Case 3:
12345678901234567890\0 (Or that of case 1 or 2 next loop)
-----
Running(*gnl_strjoin_ret):
Case 3: \n OR \0 at 20
20*cpy + \0 = 12345678901234567890\0 (new_buf overwrite)
-----
Case 3 continued: \0 at 20
remainder = 20 read - 20 = 0
01*cpy+\0 = 0\0
(buf overwrite)
(new_buf return)
END case 3:
buf = \0 (Case 3)
new_buf = 12345678901234567890\0
=====
Case 4:
\0 (Or that of case 1 or 2 next loop)
-----
Running(*gnl_strjoin_ret):
Case 4: \n OR \0 at 0
0*cpy + \0 = \0 (new_buf overwrite)
-----
Case 4 continued: \0 at 0
remainder = 0 read - 0 = 0
01*cpy+\0 = 0\0
(buf overwrite)
(new_buf return)
END case 3:
buf = \0 (Case 4)
new_buf = \0
=====
(FROM GOTO NEXT) GNL (if contains \n == FALSE)
If EOF, buf = NULL, If new_buf, return new_buf. Else, return new_buf

*/

char	*gnl_strjoin_ret(char **buf, char **new_buf, ssize_t read_len)
{
	char	*imposter_buf;
	char	*gnl_buf;
	ssize_t	suffix;
	ssize_t	char_pos;

	suffix = read_len - find_len(new_buf, '\n');
	imposter_buf = malloc(sizeof(char) * (suffix + 1));
	char_pos = find_len(buf, '\0') + read_len - suffix;
	if (!imposter_buf)
		return (NULL);
	imposter_buf[suffix] = '\0';
	while (suffix--)
		imposter_buf[suffix] = new_buf[find_len(buf, '\0') + \
read_len - suffix];
	free(buf);
	buf = imposter_buf;
	gnl_buf = malloc(sizeof(char) * (char_pos + 1));
	if (!gnl_buf)
		return (NULL);
	gnl_buf[char_pos] = '\0';
	while (char_pos--)
		gnl_buf[char_pos] = new_buf[char_pos];
	free(new_buf);
	return (gnl_buf);
}

ssize_t	find_len(const char *s, const char *look_for)
{
	size_t	len;

	len = 0;
	while (*s && *s != *look_for)
	{
		len++;
		s++;
	}
	if (*s == * look_for)
		len++;
	return (len);
}