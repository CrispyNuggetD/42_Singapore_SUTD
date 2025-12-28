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
static void			dispatch_key(unsigned char key);
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
	int		printer;
	va_list	input;
	t_spec	spec;

	printer = 0;
	va_start(input, str);
	if (!str)
		return (0);
	while (*str)
	{
		if (*str != '%')
		{
			ft_putchar_fd(*str, 1);
			str++;
			continue;
		}
		else if (*str == '%')
		{
			str++;
			parse_spec(&spec, &str);
		}
		/*
			dispatch(spec, ...)
		else
			emit literal *p
			p++
		write out all printf cases first
		read the manual	
		*/
	}
	//dispatch_key((unsigned char)key[0]);
	return (printer);
}

// The below function initialises (by mapping) the function handlers
// Variables mean type_handler, global_handler
static void	dispatch_key(unsigned char key)
{
	t_handler	fn;

	fn = get_handler((unsigned char)key);
	//if (fn)
		//fn();
}

// Parses the key specifiers with flags and everything and moves pointer
static void parse_spec(t_spec *spec, const char **ptr)
{
	while (**ptr == '-' || **ptr == '0' || **ptr == '#' || **ptr == ' ' ||
			**ptr == '+')
	{
		if (**ptr == '-')
			spec -> flags += 1;
		if (**ptr == '0')
			spec -> flags += 2;
		if (**ptr == '#')
			spec -> flags += 4;
		if (**ptr == ' ')
			spec -> flags += 8;
		if (**ptr == '+')
			spec -> flags += 16;
		(*ptr)++;
	}
	(*ptr)++;
	while (**ptr >= '0' && **ptr <= '9')
	{
		spec -> width = spec -> width * 10 + (**ptr- '0');
		(*ptr)++; 
	}
	if (**ptr == '.')
	{
		spec -> flags += 32;
		(*ptr)++;
		if (**ptr >= '0' && **ptr <= '9')
		{
			while (**ptr >= '0' && **ptr <= '9')
			{
				spec -> precision = spec -> precision * 10 + **ptr - '0';
				(*ptr)++;
			}
		}
		else
			spec -> precision = 0;
	}
	if ((**ptr == 'd' || **ptr == 'i') && **(ptr + 1))
	{	
		spec -> conversion = **ptr;
		(*ptr)++;
	}
	else
		ft_putendl_fd("Invalid conversion. To do error handling", 1)
}
// Initialise keys and also allows dispatcher to fetch those keys
/*static t_handler	init_get_handlers(unsigned char fn_keys)
{
	static t_handler const	handlers[256] = {
		['i'] = print_d_i;
		['d'] = print_d_i;
	};
	return (handlers[fn_keys]);
}*/