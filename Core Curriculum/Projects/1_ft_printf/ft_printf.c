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
static t_handler	*get_handlers(void);
static void			dispatch_key(unsigned char key);
int					ft_printf(const char *key, ...);

// Main function
int	main(void)
{
	int	len;

	len = ft_printf("i");
}
// printf("Test ft_itoa(31)\nResult is: %s", ft_itoa(31));

// This function is summoned from main to parse the input
int ft_printf(const char *key, ...)
{
	int		printer;
	va_list	list;
	t_spec	spec;

	printer = 0;
	va_start(list, arg);
	if (!key)
		return (0);
	while (*key)
	{
		if (*key == '#')
			parse_spec(&spec, &p);
		/*
			dispatch(spec, ...)
		else
			emit literal *p
			p++
		write out all printf cases first
		read the manual	
		*/
	}
	dispatch_key((unsigned char)key[0]);
	return (printer);
}

// The below function initialises (by mapping) the function handlers
// Variables mean type_handler, global_handler
static void	dispatch_key(unsigned char key)
{
	t_handler	*handlers;
	t_handler	fn;

	handlers = get_handlers();
	fn = handlers[key];
	if (fn)
		fn();
}

// Allows dispatcher to fetch initialised keys
static t_handler	*get_handlers(void)
{
	static t_handler	handlers[256];
	static int			initialized;

	if (!initialized)
	{
		handlers[(unsigned char)('i')] = print_d_i;
		handlers[(unsigned char)('d')] = print_d_i;
		initialized = 1;
	}
	return (handlers);
}
