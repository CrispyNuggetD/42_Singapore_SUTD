#include "ft_printf.h"

int	ft_printf(const char *str, ...)
{
	va_list	input;
	int		result;

	va_start(input, str);
	result = ft_vprintf_fd(1, str, &input);
	va_end(input);
	return (result);
}

int	ft_printf_fd(int fd, const char *str, ...)
{
	va_list	input;
	int		result;

	va_start(input, str);
	result = ft_vprintf_fd(fd, str, &input);
	va_end(input);
	return (result);
}
