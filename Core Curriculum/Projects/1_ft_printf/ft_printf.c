/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 13:19:21 by hnah              #+#    #+#             */
/*   Updated: 2026/01/02 13:03:01 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include <stdio.h>

// Let main stay on top (readability) with prototypes
static t_handler	init_get_handlers(unsigned char fn_keys);
static int			dispatch_key(t_context *context);
static int			printer(const char *str, int *printed, va_list *input);

// Main function
int	main(void)
{
	int	len;

	len = ft_printf("test %i %%", 123); // not working when stacked
	write(1, "\n", 1);
	ft_putnbr_fd(len, 1);
}
//intf("TesT");
// printf("Test ft_itoa(31)\nResult is: %s", ft_itoa(31));

// This function is summoned from main to parse the input
int	ft_printf(const char *str, ...)
{
	int		printed;
	va_list	input;

	printed = 0;
	if (!str)
		return (0);
	va_start(input, str);
	while (*str)
	{
		if (*str != '%')
		{
			ft_putchar_fd(*str, 1);
			str++;
			printed += 1;
			continue ;
		}
		else
			return (printer(str, &printed, &input));
	}
	va_end(input);
	return (printed);
}
// check actual error code if (!str); return (0); 
// The below function initialises (by mapping) the function handlers
// Variables mean type_handler, global_handler

static int	dispatch_key(t_context *context)
{
	t_handler	fn;
	int			len_printed;

	fn = init_get_handlers(context -> spec -> conversion);
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
	['i'] = ft_printf_d_i,
	['d'] = ft_printf_d_i,
	['%'] = ft_printf_percent,
	};

	return (handlers[fn_key]);
}

static int	printer(const char *str, int *printed, va_list *input)
{
	int			current_len;
	t_context	context;
	t_spec		spec;

	str++;
	if (!ft_printf_parse_spec(&spec, &str))
		return (error_end_stream(input));
	context.input = input;
	context.spec = &spec;
	current_len = dispatch_key(&context);
	if (current_len < 0)
		return (error_end_stream(input));
	printed += current_len;
	return (0);
}