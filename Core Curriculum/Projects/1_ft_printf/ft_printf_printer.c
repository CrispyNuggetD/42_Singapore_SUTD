/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_printer.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 02:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/01/03 02:17:11 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_printf_printer(t_context *context)
{
	ssize_t	print_success;
	
	(void)context;
	print_success = write(1, "%", 1);
	if (print_success < 0)
		return (-1);
	return (1);
}