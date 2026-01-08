/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_parse_specs.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 15:15:24 by hnah              #+#    #+#             */
/*   Updated: 2026/01/08 15:50:11 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static void	normalise_flags(t_spec *spec);
static void	get_length(t_spec *spec, const char **ptr);
static int	identify_conversion(t_spec *spec, const char **ptr);
static void	clear_set_spec_flags(t_spec *spec, const char **ptr);

// Parses the key specifiers with flags and everything and moves pointer
int	ft_printf_parse_specs(t_spec *spec, const char **ptr)
{
	clear_set_spec_flags(spec, ptr);
	if (**ptr == '\0')
		return (0);
	while (ft_isdigit(**ptr))
	{
		spec->width = spec->width * 10 + (**ptr - '0');
		(*ptr)++;
	}
	if (identify_conversion(spec, ptr) < 0)
		return (-1);
	if (spec->conversion == '%')
		spec->width = 0;
	if (spec->conversion)
		normalise_flags(spec);
	return (spec->conversion);
}

static void	clear_set_spec_flags(t_spec *spec, const char **ptr)
{
	spec->flags = 0;
	spec->width = 0;
	spec->precision = 0;
	spec->conversion = 0;
	spec->length = LEN_NONE;
	while (**ptr == '-' || **ptr == '0' || **ptr == '#' || \
**ptr == ' ' || **ptr == '+')
	{
		if (**ptr == '-')
			spec->flags |= FLAG_MINUS;
		else if (**ptr == '0')
			spec->flags |= FLAG_ZERO;
		else if (**ptr == '#')
			spec->flags |= FLAG_HASH;
		else if (**ptr == ' ')
			spec->flags |= FLAG_SPACE;
		else if (**ptr == '+')
			spec->flags |= FLAG_PLUS;
		(*ptr)++;
	}
}

static int	identify_conversion(t_spec *spec, const char **ptr)
{
	if (**ptr == '.')
	{
		spec->flags |= FLAG_PREC;
		(*ptr)++;
		while (ft_isdigit(**ptr))
		{
			spec->precision = spec->precision * 10 + (**ptr - '0');
			(*ptr)++;
		}
	}
	get_length(spec, ptr);
	if (**ptr == 'd' || **ptr == 'i' || **ptr == '%' || **ptr == 'c' || \
**ptr == 's' || **ptr == 'u' || **ptr == 'x' || **ptr == 'X' || **ptr == 'p')
	{
		spec->conversion = **ptr;
		(*ptr)++;
	}
	else
		return (-1);
	return (0);
}

static void	get_length(t_spec *spec, const char **ptr)
{
	{
		if ((*ptr)[0] == 'h')
		{
			if ((*ptr)[1] == 'h')
			{
				spec->length = LEN_HH;
				(*ptr)++;
			}
			else
				spec->length = LEN_H;
		}
		else if ((*ptr)[0] == 'l')
		{
			if ((*ptr)[1] == 'l')
			{
				spec->length = LEN_LL;
				(*ptr)++;
			}
			else
				spec->length = LEN_L;
		}
		(*ptr)++;
	}
}

static void	normalise_flags(t_spec *spec)
{
	if (spec->flags & FLAG_MINUS)
		spec->flags &= ~FLAG_ZERO;
	if ((spec->flags & FLAG_PREC) && is_numeric_conv(spec->conversion))
		spec->flags &= ~FLAG_ZERO;
	if (spec->flags & FLAG_PLUS)
		spec->flags &= ~FLAG_SPACE;
}
/*
Conflicts (one wins):
- (left align) overrides 0 (zero padding) → 
if both present, you pad with spaces, not zeros.
+ overrides space flag → you don’t get both; + wins.
For integers, having a precision usually disables 0 
padding (because precision controls leading zeros more explicitly).
*/
