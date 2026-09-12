#include "ryker_ft.h"

static int	ryker_ft_repeat(t_ryker_output *out, char c, size_t count)
{
	while (count--)
	{
		if (ryker_ft_write(out, &c, 1) < 0)
			return (-1);
	}
	return (0);
}

int	ryker_ft_print(t_ryker_output *out, t_spec *spec, t_print *paper)
{
	size_t	length;

	length = (paper->sign != 0) + paper->prefix_len
		+ paper->prec_zeros + paper->core_len;
	if ((size_t)spec->width > length)
		paper->pad_len = (size_t)spec->width - length;
	if (spec->flags & FLAG_ZERO)
		paper->pad_char = '0';
	if (!(spec->flags & FLAG_MINUS) && paper->pad_char == ' '
		&& ryker_ft_repeat(out, ' ', paper->pad_len) < 0)
		return (-1);
	if (paper->sign && ryker_ft_write(out, &paper->sign, 1) < 0)
		return (-1);
	if (ryker_ft_write(out, paper->prefix, paper->prefix_len) < 0)
		return (-1);
	if (!(spec->flags & FLAG_MINUS) && paper->pad_char == '0'
		&& ryker_ft_repeat(out, '0', paper->pad_len) < 0)
		return (-1);
	if (ryker_ft_repeat(out, '0', paper->prec_zeros) < 0
		|| ryker_ft_write(out, paper->core, paper->core_len) < 0)
		return (-1);
	if (spec->flags & FLAG_MINUS)
		return (ryker_ft_repeat(out, ' ', paper->pad_len));
	return (0);
}
