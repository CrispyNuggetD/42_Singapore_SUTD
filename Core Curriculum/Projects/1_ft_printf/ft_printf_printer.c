/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_printer.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 02:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/01/03 07:32:49 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_printf_printer(t_context *context)
{
	char	*text;
	int		output;
	size_t	output_len;
	ssize_t	print_success;
	
	output = va_arg(*(context -> input), int);
	text = ft_itoa(output); // Change to no malloc later
	if (!text)
		return (-1);
	output_len = ft_strlen(text);
	print_success = write(1, text, output_len);
	free(text);
	if (print_success < 0)
		return (-1);
	return (output_len);
}

/*
3) Decide ownership (important!)

t_print should not own memory. It’s just “here’s what to print”.
So: if core comes from malloc (ft_itoa), the handler frees it after ft_print_printer() returns.
*/