/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_iterative_factorial.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 18:16:29 by hnah              #+#    #+#             */
/*   Updated: 2025/08/12 18:34:40 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Multiply resuly by nb while decrementing nb!! COool!
int	ft_iterative_factorial(int nb)
{
	int	result;

	result = 1;
	if (nb < 0)
		return (0);
	while (nb > 1)
		result *= nb--;
	return (result);
}
