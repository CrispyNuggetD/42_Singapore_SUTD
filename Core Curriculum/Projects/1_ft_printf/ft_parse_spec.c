/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_parse_spec.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 15:15:24 by hnah              #+#    #+#             */
/*   Updated: 2025/12/29 15:15:30 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static void	clear_set_spec_flags(t_spec *spec, const char **ptr);
static void	normalise_flags(t_spec *spec);
static int	is_numeric_conv(int conversion);




// Parses the key specifiers with flags and everything and moves pointer
int	parse_spec(t_spec *spec, const char **ptr)
{
	clear_set_spec_flags(spec, ptr);
	while (ft_isdigit(**ptr))
	{
		spec -> width = spec -> width * 10 + (**ptr - '0');
		(*ptr)++; 
	}
	if (**ptr == '.')
	{
		spec -> flags |= FLAG_PREC;
		(*ptr)++;
		while (ft_isdigit(**ptr))
		{
			spec -> precision = spec -> precision * 10 + (**ptr - '0');
			(*ptr)++;
		}
	}
	if ((**ptr == 'd' || **ptr == 'i'))
	{	
		spec -> conversion = **ptr;
		(*ptr)++;
	}
	if (**ptr == '%')
	{
		ft_putchar_fd('%');
		(*ptr)++;
	}
	else
		ft_putendl_fd("Invalid conversion. Do error handling.", 1);
	normalise_flags(spec);
}

static void	clear_set_spec_flags(t_spec *spec, const char **ptr)
{
	spec -> flags = 0;
	spec -> width = 0;
	spec -> precision = 0;
	spec -> conversion = 0;
	while (**ptr == '-' || **ptr == '0' || **ptr == '#' || **ptr == ' ' ||
			**ptr == '+')
	{
		if (**ptr == '-')
			spec -> flags |= FLAG_MINUS;
		else if (**ptr == '0')
			spec -> flags |= FLAG_ZERO;
		else if (**ptr == '#')
			spec -> flags |= FLAG_HASH;
		else if (**ptr == ' ')
			spec -> flags |= FLAG_SPACE;
		else if (**ptr == '+')
			spec -> flags |= FLAG_PLUS;
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
		spec -> flags &= ~FLAG_SPACE;
}

static int	is_numeric_conv(int conversion)
{
	if ((conversion) == 'd' || (conversion) == 'i' || (conversion) == 'u' ||
		(conversion) == 'x' || (conversion) == 'X')
		return (1);
	return (0);
}

// && is_numeric(spec->conversion)
// Needed as precision suppresses 0 padding only for numeric conversions