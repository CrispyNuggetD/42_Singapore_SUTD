/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ryker_ft_abs.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 22:50:13 by hnah              #+#    #+#             */
/*   Updated: 2026/09/23 22:50:14 by hnah             ###   ########.fr       */
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
