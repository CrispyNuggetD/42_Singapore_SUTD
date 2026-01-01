/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 13:19:21 by hnah              #+#    #+#             */
/*   Updated: 2025/12/26 11:18:32 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include <stdio.h>

// Let main stay on top (readability) with prototypes
static t_handler	init_get_handlers(unsigned char fn_keys);
static void			dispatch_key(t_spec *spec, va_list *input);
int					ft_printf(const char *key, ...);

// Main function
int	main(void)
{
	int	len;

	printf("TesT");
	//len = ft_printf("i");
}
// printf("Test ft_itoa(31)\nResult is: %s", ft_itoa(31));

// This function is summoned from main to parse the input
int ft_printf(const char *str, ...)
{
	int			printed;
	int			parsing_passed;
	int			current_len;
	t_context	context;
	t_spec		spec;
	va_list		input;

	printed = 0;
	if (!str)
		return (0); // check actual error code
	va_start(input, str);
	while (*str)
	{
		if (*str != '%')
		{
			ft_putchar_fd(*str, 1);
			str++;
			printed += 1;
		}
		else if (*str == '%')
		{
			str++;
			parsing_passed = parse_spec(&spec, &str);
			if (parsing_passed)
			{
				context.input = &input;
				context.spec = &spec;
				current_len = dispatch_key(&context);
				if (current_len < 0)
				{
					va_end(input);
					return (-1);
				}
				printed += current_len;
			}
			else
			{
				va_end(input);
				return (-1);
			}
		}
	}
	va_end(input);
	return (printed);
}

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
		['i'] = print_d_i,
		['d'] = print_d_i,
	};
	return (handlers[fn_key]);
}