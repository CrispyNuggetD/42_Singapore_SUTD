/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 13:19:21 by hnah              #+#    #+#             */
/*   Updated: 2026/01/09 18:25:26 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
//#include <stdio.h>

// Let main stay on top (readability) with prototypes
static t_handler	init_get_handlers(unsigned char fn_keys);
static int			dispatch_key(t_context *context);
static int			main_coordinator(const char **str, t_context *context, \
va_list *input);

// This function is summoned from the variadic entry points
int	ft_vprintf_fd(int fd, const char *str, va_list *input)
{
	t_context	context;

	ft_printf_init_t_context(&context);
	if (!str)
		return (-1);
	context.fd = fd;
	while (*str)
	{
		if (*str != '%')
		{
			if (write_guaranteed(&context, str, 1) < 0)
				return (-1);
			str++;
		}
		else
		{
			if (main_coordinator(&str, &context, input) < 0)
				return (-1);
		}
	}
	return (context.printed);
}

static int	main_coordinator(const char **str, t_context *context,\
			va_list *input)
{
	int			dispatch_success;
	t_spec		spec;

	(*str)++;
	if (!ft_printf_parse_specs(&spec, str))
		return (-1);
	context->input = input;
	context->spec = &spec;
	dispatch_success = dispatch_key(context);
	if (dispatch_success < 0)
		return (-1);
	return (0);
}
// check actual error code if (!str); return (0); 
// The below function initialises (by mapping) the function handlers
// Variables mean type_handler, global_handler

static int	dispatch_key(t_context *context)
{
	t_handler	fn;
	int			len_printed;

	fn = init_get_handlers(context->spec->conversion);
	if (fn)
		len_printed = fn(context);
	else
		return (-1);
	return (len_printed);
}

// Initialise keys and also allows dispatcher to fetch those keys
static t_handler	init_get_handlers(unsigned char fn_key)
{
	static t_handler const	handlers[256] = {
	['%'] = ft_printf_percent,
	['d'] = ft_printf_d_i,
	['i'] = ft_printf_d_i,
	['u'] = ft_printf_unsigned_int,
	['s'] = ft_printf_string,
	['c'] = ft_printf_character,
	['x'] = ft_printf_hex_small,
	['X'] = ft_printf_hex_big,
	['p'] = ft_printf_pointer,
	};

	return (handlers[fn_key]);
}
