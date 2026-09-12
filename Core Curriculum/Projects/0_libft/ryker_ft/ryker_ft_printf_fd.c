#include "ryker_ft.h"

static int	ryker_ft_format(t_ryker_output *out, const char *format,
		va_list *args)
{
	t_spec	spec;

	while (*format)
	{
		if (*format != '%')
		{
			if (ryker_ft_write(out, format++, 1) < 0)
				return (-1);
		}
		else
		{
			format++;
			if (ryker_ft_parse(&spec, &format, args) <= 0)
				return (-1);
			if (ryker_ft_conversion(out, &spec, args) < 0)
				return (-1);
		}
	}
	return (out->printed);
}

int	ryker_ft_printf_fd(int fd, const char *format, ...)
{
	t_ryker_output	out;
	va_list			args;
	int				result;

	if (!format)
		return (-1);
	out.fd = fd;
	out.printed = 0;
	va_start(args, format);
	result = ryker_ft_format(&out, format, &args);
	va_end(args);
	return (result);
}
