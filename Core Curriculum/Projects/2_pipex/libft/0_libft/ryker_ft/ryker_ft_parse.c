#include "ryker_ft.h"

static int	ryker_ft_field(const char **format, va_list *args, int *value)
{
	*value = 0;
	if (**format == '*')
	{
		*value = va_arg(*args, int);
		(*format)++;
		return (0);
	}
	while (ft_isdigit(**format))
	{
		if (*value > (INT_MAX - (**format - '0')) / 10)
			return (-1);
		*value = *value * 10 + (*(*format)++ - '0');
	}
	return (0);
}

static void	ryker_ft_decimal(char **dest, int value)
{
	char	buffer[10];
	int		length;

	length = 0;
	buffer[length++] = '0' + value % 10;
	value /= 10;
	while (value)
	{
		buffer[length++] = '0' + value % 10;
		value /= 10;
	}
	while (length)
		*(*dest)++ = buffer[--length];
}

static int	ryker_ft_dimensions(const char **format, char **dest,
		va_list *args)
{
	int	value;

	if (ryker_ft_field(format, args, &value) < 0 || value == INT_MIN)
		return (-1);
	if (value < 0)
	{
		*(*dest)++ = '-';
		value = -value;
	}
	ryker_ft_decimal(dest, value);
	if (**format == '.')
	{
		(*format)++;
		if (ryker_ft_field(format, args, &value) < 0)
			return (-1);
		if (value >= 0)
		{
			*(*dest)++ = '.';
			ryker_ft_decimal(dest, value);
		}
	}
	return (0);
}

int	ryker_ft_parse(t_spec *spec, const char **format, va_list *args)
{
	char		buffer[96];
	char		*dest;
	const char	*parsed;

	dest = buffer;
	while (**format && ft_strchr("-0# +", **format))
	{
		if (dest - buffer >= 64)
			return (-1);
		*dest++ = *(*format)++;
	}
	if (ryker_ft_dimensions(format, &dest, args) < 0)
		return (-1);
	if (**format == 'h' || **format == 'l')
	{
		*dest++ = *(*format)++;
		if (**format == dest[-1])
			*dest++ = *(*format)++;
	}
	if (!**format)
		return (-1);
	*dest++ = *(*format)++;
	*dest = '\0';
	parsed = buffer;
	return (ft_printf_parse_specs(spec, &parsed));
}
