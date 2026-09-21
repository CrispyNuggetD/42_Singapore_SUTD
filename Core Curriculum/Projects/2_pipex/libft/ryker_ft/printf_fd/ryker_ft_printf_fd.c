/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ryker_ft_printf_fd.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 05:27:07 by hnah              #+#    #+#             */
/*   Updated: 2026/09/14 03:44:33 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ryker_ft_printf.h"

int	ryker_ft_printf_fd(int fd, const char *str, ...)
{
	va_list	input;
	int		previous_fd;
	int		result;

	if (fd < 0)
		return (-1);
	previous_fd = ft_printf_fd_setting(-1);
	ft_printf_fd_setting(fd);
	va_start(input, str);
	result = ft_vprintf(str, &input);
	va_end(input);
	ft_printf_fd_setting(previous_fd);
	return (result);
}
