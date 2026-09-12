#include "ryker_ft.h"

int	ryker_ft_write(t_ryker_output *out, const char *text, size_t length)
{
	ssize_t	written;

	if (length > (size_t)(INT_MAX - out->printed))
	{
		errno = EOVERFLOW;
		return (-1);
	}
	while (length)
	{
		written = write(out->fd, text, length);
		if (written < 0 && errno == EINTR)
			continue ;
		if (written <= 0)
			return (-1);
		out->printed += written;
		text += written;
		length -= written;
	}
	return (0);
}
