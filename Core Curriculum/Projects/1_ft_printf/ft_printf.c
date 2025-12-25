/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 13:19:21 by hnah              #+#    #+#             */
/*   Updated: 2025/12/25 15:36:48 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "ft_printf.h"

int	ft_printf(const char *input, ...)
{
	int	printer;

	printer = 0;
	(void)input;
	return (printer);
}

int	main(void)
{
	printf("Test ft_itoa(31)\nResult is: %s", ft_itoa(31));
	return (0);
}
