/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_parse_specs.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 15:15:24 by hnah              #+#    #+#             */
/*   Updated: 2026/01/08 12:16:12 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	is_numeric_conv(int conversion);
static void	normalise_flags(t_spec *spec);
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

static int	is_numeric_conv(int conversion)
{
	if ((conversion) == 'd' || (conversion) == 'i' || (conversion) == 'u' || \
(conversion) == 'x' || (conversion) == 'X')
		return (1);
	return (0);
}

// && is_numeric(spec->conversion)
// Needed as precision suppresses 0 padding only for numeric conversions