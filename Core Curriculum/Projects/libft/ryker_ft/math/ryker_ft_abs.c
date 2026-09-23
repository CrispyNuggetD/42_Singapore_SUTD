/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ryker_ft_abs.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 12:13:58 by hnah              #+#    #+#             */
/*   Updated: 2025/12/11 13:50:31 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ryker_ft.h"

/* Precondition: one must not be INT_MIN; its magnitude cannot fit in int. */
int	ryker_ft_abs(int one)
{
	if (one >= 0)
		return (one);
	return (-one);
}
