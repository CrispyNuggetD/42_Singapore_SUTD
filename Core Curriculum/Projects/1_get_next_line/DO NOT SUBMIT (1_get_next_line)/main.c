#include "get_next_line.h"
#include <stdio.h>
#include <fcntl.h> 

static int gnl_atoi(char *v)
{
	int i = 0;
	int a = 0;
	while (v[i])
	{
		a *= 10;
		a += v[i] - '0';
		i ++;
	}
	if (!a)
		write(1, "invalid integer\n", 4);
	return (a);
}

int main(int c, char **v)
{
	int	i = 0;
	if (c < 3)
		return (1);
	int fd = open(v[1], O_RDONLY); //change to '0' for stdin
	if (fd < 0)
		return ((write(1, "u no the allow open\n", 20)), 1);
	int line = gnl_atoi(v[2]);
	char *the_line = NULL;
	while (line)
	{
	    i = 0;
		the_line = get_next_line(fd);
		if (the_line == NULL)
			return ((write(1, "finish le\n", 10)), (close(fd)), 1);
		while (the_line[i])
			i++;
		write(1, the_line, i);
		line--;
		free(the_line);
	}
	close(fd);
}
