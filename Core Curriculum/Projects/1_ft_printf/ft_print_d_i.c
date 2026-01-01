/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_handler.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 09:34:40 by hnah              #+#    #+#             */
/*   Updated: 2025/12/26 09:54:26 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	print_d_i(void *context)
{
	char	*str;
	int		str_len;
	
	str_len = ft_strlen(str);
	str = "test print of print_d_i\n";
	write(1, str, str_len);
	return (str_len);
}