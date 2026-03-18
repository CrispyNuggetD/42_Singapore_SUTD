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
static int			error_and_return(va_list *input);
static int			main_coordinator(const char **str, t_context *context, \
va_list *input);

// Main function
/* int	main(void)
{
	int	mylen;
	int	printf_len;

	printf(" ===== MANDATORY BELOW ===== \n");
	printf("\n ===== STRINGS ===== \n");
	ft_printf("My output is:\n");
	mylen = ft_printf("% aewfa e %s awer", "test");
	printf("\nMy ft_printf return value:%i\n\n", mylen);

	ft_printf("Actual printf output is:\n");
	printf_len = printf("% aewfa e %s awer", "test");
	printf("\nActual printf return value:%i\n\n", printf_len);

	printf("\n ===== POINTERS MIN MAX ===== \n");
	ft_printf("My output is:\n");
	mylen = ft_printf(" %p %p ", LONG_MIN, LONG_MAX);
	printf("\nMy ft_printf return value:%i\n\n", mylen);

	ft_printf("Actual printf output is:\n");
	printf_len = printf(" %p %p ", LONG_MIN, LONG_MAX);
	printf("\nActual printf return value:%i\n\n", printf_len);

	printf("\n\n\n ===== BONUSES BELOW ===== \n");
	printf("\n ===== INVALID INPUT ===== \n");
	ft_printf("My output is:\n");
	mylen = ft_printf("%5%");
	printf("\nMy ft_printf return value:%i\n\n", mylen);

	ft_printf("Actual printf output is:\n");
	printf_len = printf("%5%");
	printf("\nActual printf return value:%i\n\n", printf_len);

	printf("\n ===== STR POINTER WITH PADDING ===== \n");
	ft_printf("My output is:\n");
	mylen = ft_printf("|%05p|", (void*)0x1);
	printf("\nMy ft_printf return value:%i\n\n", mylen);

	ft_printf("Actual printf output is:\n");
	printf_len = printf("|%05p|", (void*)0x1);
	printf("\nActual printf return value:%i\n\n", printf_len);

	printf("\n ===== DECIMAL PAD AND PRECISION ===== \n");
	ft_printf("My output is:\n");
	mylen = ft_printf("%08.5d", 42);
	printf("\nMy ft_printf return value:%i\n\n", mylen);

	ft_printf("Actual printf output is:\n");
	printf_len = printf("%08.5d", 42);
	printf("\nActual printf return value:%i\n\n", printf_len);

	printf("\n ===== COMPLEX EXAMPLES ===== \n");
	ft_printf("My output is:\n");
	mylen = ft_printf("%#37llXabc42isgreat_42^&NULL %6.3hx", 
	\
522337203685470ull, 12642);
	printf("\nMy ft_printf return value:%i\n\n", mylen);

	ft_printf("Actual printf output is:\n");
	printf_len = printf("%#37llXabc42isgreat_42^&NULL %6.3hx", 
	\
522337203685470ull, 12642);
	printf("\nActual printf return value:%i\n\n", printf_len);
} */

// This function is summoned from main to parse the input
int	ft_printf(const char *str, ...)
{
	t_context	context;
	va_list		input;

	ft_printf_init_t_context(&context);
	if (!str)
		return (-1);
	va_start(input, str);
	while (*str)
	{
		if (*str != '%')
		{
			if (write_guaranteed(&context, str, 1) < 0)
				return (error_and_return(&input));
			str++;
		}
		else
		{
			if (main_coordinator(&str, &context, &input) < 0)
				return (error_and_return(&input));
		}
	}
	va_end(input);
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

static int	error_and_return(va_list *input)
{
	va_end(*input);
	return (-1);
}

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
