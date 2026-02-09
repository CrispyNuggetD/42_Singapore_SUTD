/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 13:19:21 by hnah              #+#    #+#             */
/*   Updated: 2026/02/09 14:58:20 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"


/* ssize_t	newline_index(char *str)
{
	ssize_t len;
	int		found;

	len = 0;
	found = -1;
	while (*str)
	{
		if (*str == '\n')
		{
			found = 1;
			break;
		}
		len++;
		str++;
	}
	if (found)
		return (len);
	return (-1);
} */

char	*stash_append(char **stash, char *buf, ssize_t nread)
{

}

/* char	*stash_trim_ret(char **stash)
{
	ssize_t	len;
	int		cut;
	char	*stash_backup;
	char	*ret;
	char	*remainder;

	len = 0;
	stash_backup = *stash;
	if (!stash_backup || *stash_backup == '\0')
		return (NULL);
	len = ft_strlen(stash_backup);
	cut = newline_index(stash_backup) + 1;
	if (newline_index(stash_backup) < 0)
		cut = len;
	ret = malloc(sizeof(char) * (cut + 1));
	if (!ret)
		return (NULL);
	ret[cut] = '\0';
	ft_memcpy(ret, stash_backup, cut);
	if (len - cut == 0)
		remainder = NULL;
	else
		remainder = malloc(sizeof(char) * (len - cut + 1));
	if (!remainder)
		return (NULL);
	remainder[len - cut] = '\0';
	ft_memcpy(remainder, stash_backup + cut, len - cut);
	free(stash_backup);
	*stash = remainder;
	return (ret);
} */

/* ssize_t	ft_strlen(const char *s)
{
	size_t	len;

	len = 0;
	while (*s)
	{
		len++;
		s++;
	}
	return (len);
} */

/* void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	const unsigned char	*src_backup;
	unsigned char		*dest_backup;

	if (!dest && !src)
		return (NULL);
	src_backup = (unsigned char *)src;
	dest_backup = (unsigned char *)dest;
	while (n--)
	{
		*dest_backup = *src_backup;
		dest_backup++;
		src_backup++;
	}
	return (dest);
}
 */
/* int	check_char(char *new_stash)
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
} */

/*
*gnl_strjoin_ret always frees temporary stash used by overwriting before returning.
if i want to use for both, 2 cases, 1 continue (no /n) another is yes (\n)
Cases: GNL (if contains \n == TRUE, Immediately):
Case 1:
12\n45678\0x (len might be < BUFFER_SIZE + 1 but definitely \0)
Running(*gnl_strjoin_ret):

Else Cases...: 
Case 2,3: GNL continue (GOTO)

=====
B: Find index (+1 if \n):
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
Case 1: \n OR \0 at [2]
(2+1)+1 = 4
3* cpy + \0 = 12\n\0 (new_buf overwrite)
-----

Case 1:
12\n45678\0x


Case 1 continued: \0 at 8
remainder = 8 input - 3 = 5
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
remainder = 5 input - 5 = 0
01*cpy+\0 = 0\0
(buf overwrite)
(new_buf return)
END case 2:
buf = \0 (Case 2)
new_buf = 12345\0
=====
Case 3 none:
12345678901234567890\0 (Or that of case 1 or 2 next loop)
-----
Running(*gnl_strjoin_ret):
Case 3: \n OR \0 at 19
if read != buffer size:
19*cpy + \0 = 2345678901234567890\0 (new_buf overwrite)
-----
Case 3 continued: 
else: \0 at 19
remainder = 19 input - 19 = 0
01*cpy+\0 = 0\0
(buf overwrite)
(new_buf return)
END case 3:
buf = \0 (Case 3)
new_buf = 2345678901234567890\0
=====
Case 4 during entry of loop:
\0 (Or that of case 1 or 2 next loop)
	s = *stash;
	if (!s || !*s)
		return (NULL);
	len = ft_strlen(s);
	cut = newline_index(s);
	if (cut < 0)
		cut = len;
	else
		cut++;
	ret = malloc(cut + 1);
	if (!ret)
		return (NULL);
	ft_memcpy(ret, s, cut);
	ret[cut] = '\0';
	if (cut == len)
		return (free(s), *stash = NULL, ret);
	rem = malloc(len - cut + 1);
	if (!rem)
		return (free(ret), NULL);
	ft_memcpy(rem, s + cut, len - cut);
	s = *stash;
	if (!s || !*s)
		return (NULL);
	len = ft_strlen(s);
	cut = newline_index(s);
	if (cut < 0)
		cut = len;
	else
		cut++;
	ret = malloc(cut + 1);
	if (!ret)
		return (NULL);
	ft_memcpy(ret, s, cut);
	ret[cut] = '\0';
	if (cut == len)
		return (free(s), *stash = NULL, ret);
	rem = malloc(len - cut + 1);
	if (!rem)
		return (free(ret), NULL);
	ft_memcpy(rem, s + cut, len - cut);
	rem[len - cut] = '\0';
	free(s);
	*stash = rem;
	return (r
	rem[len - cut] = '\0';
	free(s);
	*stash = rem;
	return (r
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
If EOF, buf = NULL, If new_buf, return new_buf. Else, if new_buf is null doesn't that mean EOF

Dual use split_swap_return(char **buf, char **new, ssize_t nread)
//single dual-meaning return for 25 lines. I mean you just ignore the return if not needed
//new = buf + newly read
make a temp_buf;
optional: 
if (\n exist){
whatever not before an \n (i.e. or after) to temp_buf;
whatever that exist until "an \n", free new, malloc and copy to new;
set buf to temp_buf}
else {set buf to new}
also return (new) // ignore in base GNL function if ran at start to check \n, else return as the GNL line


*/

/*char	*gnl_strjoin_ret(char **buf, char **new_buf, ssize_t read_len)
{
	char	*temp_buf;
	char	*gnl_buf;
	ssize_t	suffix;
	ssize_t	char_pos;

	suffix = read_len - find_len(new_buf, '\n');
	temp_buf = malloc(sizeof(char) * (suffix + 1));
	char_pos = find_len(buf, '\0') + read_len - suffix;
	if (!temp_buf)
		return (NULL);
	temp_buf[suffix] = '\0';
	while (suffix--)
		temp_buf[suffix] = new_buf[find_len(buf, '\0') + \
read_len - suffix];
	free(buf);
	buf = temp_buf;
	gnl_buf = malloc(sizeof(char) * (char_pos + 1));
	if (!gnl_buf)
		return (NULL);
	gnl_buf[char_pos] = '\0';
	while (char_pos--)
		gnl_buf[char_pos] = new_buf[char_pos];
	free(new_buf);
	return (gnl_buf);
}*/

char	*newline_ret(char **buf)
{
	char	*temp_buf;
	char	*gnl_buf;
	ssize_t	prefix;
	ssize_t	suffix;

	prefix = find_len(*buf, '\n');
	suffix = find_len(*buf, '\0') - prefix;
	temp_buf = malloc(sizeof(char) * (suffix + 1));
	if (!temp_buf)
		return (NULL);
	temp_buf[suffix] = '\0';
	while (suffix--)
		temp_buf[suffix] = (*buf)[prefix + suffix];
	gnl_buf = malloc(sizeof(char) * (prefix + 1));
	if (!gnl_buf)
	{
		free(temp_buf);
		return (NULL);
	}
	gnl_buf[prefix] = '\0';
	while (prefix--)
		gnl_buf[prefix] = (*buf)[prefix];
	free(*buf);
	*buf = temp_buf;
	return (gnl_buf);
}

char	*gnl_strjoin(char *buf, char *new_buf)
{
	char	*temp_buf;
	ssize_t	new_buf_len;
	ssize_t	buf_len;
	
	new_buf_len = find_len(new_buf, '\0');
	buf_len = find_len(buf, '\0');
	temp_buf = malloc(sizeof(char) * (new_buf_len + buf_len + 1));
	if(!temp_buf)
		return (NULL);
	temp_buf[new_buf_len + buf_len] = '\0';
	while(new_buf_len--)
		temp_buf[buf_len + new_buf_len] = new_buf[new_buf_len];
	while(buf_len--)
		temp_buf[buf_len] = buf[buf_len];
	return (temp_buf);
}

ssize_t	find_len(const char *s, int look_for)
{
	ssize_t	len;

	len = 0;
	if (!s)
		return (0);
	while (*s && *s != look_for)
	{
		len++;
		s++;
	}
	if (*s && *s == look_for)
		len++;
	return (len);
}