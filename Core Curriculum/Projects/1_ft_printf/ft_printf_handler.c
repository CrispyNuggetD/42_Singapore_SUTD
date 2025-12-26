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

static void	print_d_i(void)
{
	char	*str;

	str = "print_d_i\n";
	write(1, str, ft_strlen(str));
	return ;
}
