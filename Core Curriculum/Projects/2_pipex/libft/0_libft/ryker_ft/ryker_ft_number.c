#include "ryker_ft.h"

static long long	ryker_ft_signed(va_list *args, t_length length)
{
	if (length == LEN_LL)
		return (va_arg(*args, long long));
	if (length == LEN_L)
		return (va_arg(*args, long));
	if (length == LEN_H)
		return ((short)va_arg(*args, int));
	if (length == LEN_HH)
		return ((signed char)va_arg(*args, int));
	return (va_arg(*args, int));
}

static unsigned long long	ryker_ft_unsigned(va_list *args, t_length length)
{
	if (length == LEN_LL)
		return (va_arg(*args, unsigned long long));
	if (length == LEN_L)
		return (va_arg(*args, unsigned long));
	if (length == LEN_H)
		return ((unsigned short)va_arg(*args, unsigned int));
	if (length == LEN_HH)
		return ((unsigned char)va_arg(*args, unsigned int));
	return (va_arg(*args, unsigned int));
}

static int	ryker_ft_digits(t_ryker_output *out, t_spec *spec,
		t_print *paper, unsigned long long value)
{
	char		buffer[64];
	const char	*digits;
	size_t		base;
	size_t		index;

	digits = "0123456789abcdef";
	if (spec->conversion == 'X')
		digits = "0123456789ABCDEF";
	base = 10;
	if (spec->conversion == 'x' || spec->conversion == 'X'
		|| spec->conversion == 'p')
		base = 16;
	index = sizeof(buffer);
	if (value || !(spec->flags & FLAG_PREC) || spec->precision)
		buffer[--index] = digits[value % base];
	while (value / base)
	{
		value /= base;
		buffer[--index] = digits[value % base];
	}
	paper->core = buffer + index;
	paper->core_len = sizeof(buffer) - index;
	if ((spec->flags & FLAG_PREC)
		&& (size_t)spec->precision > paper->core_len)
		paper->prec_zeros = spec->precision - paper->core_len;
	return (ryker_ft_print(out, spec, paper));
}

static unsigned long long	ryker_ft_value(t_spec *spec, t_print *paper,
		va_list *args)
{
	long long	value;

	if (spec->conversion == 'p')
		return ((uintptr_t)va_arg(*args, void *));
	if (spec->conversion != 'd' && spec->conversion != 'i')
		return (ryker_ft_unsigned(args, spec->length));
	value = ryker_ft_signed(args, spec->length);
	if (value < 0)
	{
		paper->sign = '-';
		return (0ULL - (unsigned long long)value);
	}
	if (spec->flags & FLAG_PLUS)
		paper->sign = '+';
	else if (spec->flags & FLAG_SPACE)
		paper->sign = ' ';
	return (value);
}

int	ryker_ft_number(t_ryker_output *out, t_spec *spec, va_list *args)
{
	t_print				paper;
	unsigned long long	value;

	ft_printf_init_t_print(&paper);
	value = ryker_ft_value(spec, &paper, args);
	if (spec->conversion == 'p' && !value)
	{
		paper.core = "(nil)";
		paper.core_len = 5;
		return (ryker_ft_print(out, spec, &paper));
	}
	if (value && (spec->conversion == 'p' || ((spec->flags & FLAG_HASH)
			&& (spec->conversion == 'x' || spec->conversion == 'X'))))
	{
		paper.prefix = "0x";
		if (spec->conversion == 'X')
			paper.prefix = "0X";
		paper.prefix_len = 2;
	}
	return (ryker_ft_digits(out, spec, &paper, value));
}
