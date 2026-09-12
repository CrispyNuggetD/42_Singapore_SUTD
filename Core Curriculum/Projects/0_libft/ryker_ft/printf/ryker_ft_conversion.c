#include "ryker_ft_printf.h"

int	ryker_ft_conversion(t_ryker_output *out, t_spec *spec, va_list *args)
{
	t_print	paper;
	char	character;

	if (is_numeric_conv(spec->conversion) || spec->conversion == 'p')
		return (ryker_ft_number(out, spec, args));
	if (spec->length != LEN_NONE)
		return (-1);
	ft_printf_init_t_print(&paper);
	if (spec->conversion == 's')
	{
		paper.core = va_arg(*args, char *);
		if (!paper.core)
			paper.core = "(null)";
		paper.core_len = ft_strlen(paper.core);
		if ((spec->flags & FLAG_PREC)
			&& (size_t)spec->precision < paper.core_len)
			paper.core_len = spec->precision;
	}
	else
	{
		character = '%';
		if (spec->conversion == 'c')
			character = (char)va_arg(*args, int);
		paper.core = &character;
		paper.core_len = 1;
	}
	return (ryker_ft_print(out, spec, &paper));
}
