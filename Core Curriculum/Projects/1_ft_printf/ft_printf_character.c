/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_character.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 13:55:30 by hnah              #+#    #+#             */
/*   Updated: 2026/01/07 19:36:12 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_printf_character(t_context *context)
{
	char	c;
	int		arg;
	t_print	paper;

	ft_printf_init_t_print(&paper);
	arg = va_arg(*(context -> input), int);
	c = (char)arg;
	paper.core = &c;
	paper.core_len = 1;
	return (ft_printf_print_config(context, &paper));
}
