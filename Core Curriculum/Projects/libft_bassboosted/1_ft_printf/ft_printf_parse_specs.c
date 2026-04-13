/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_parse_specs.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 15:15:24 by hnah              #+#    #+#             */
/*   Updated: 2026/01/09 02:08:19 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static void	clear_spec(t_spec *spec);
static void	get_length(t_spec *spec, const char **ptr);

// Parses only the conversion (mandatory part)
int	ft_printf_parse_specs(t_spec *spec, const char **ptr)
{
	clear_spec(spec);
	if (**ptr == '\0')
		return (0);
	get_length(spec, ptr);
	if (**ptr == 'd' || **ptr == 'i' || **ptr == '%' || **ptr == 'c' || \
**ptr == 's' || **ptr == 'u' || **ptr == 'x' || **ptr == 'X' || **ptr == 'p')
	{
		spec->conversion = **ptr;
		(*ptr)++;
		return (spec->conversion);
	}
	return (-1);
}

static void	clear_spec(t_spec *spec)
{
	spec->flags = 0;
	spec->width = 0;
	spec->precision = 0;
	spec->conversion = 0;
	spec->length = LEN_NONE;
}

static void	get_length(t_spec *spec, const char **ptr)
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
		(*ptr)++;
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
		(*ptr)++;
	}
}
