/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_d_i.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 09:34:40 by hnah              #+#    #+#             */
/*   Updated: 2026/01/03 13:45:11 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_printf_d_i(t_context *context)
{
	char	*text;
	int		output;
	size_t	output_len;
	ssize_t	print_success;
	t_print paper;

	init_t_print(&paper);	

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