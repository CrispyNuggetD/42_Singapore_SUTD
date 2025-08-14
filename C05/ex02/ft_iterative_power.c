/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_iterative_power.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 19:15:56 by hnah              #+#    #+#             */
/*   Updated: 2025/08/13 12:21:36 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_iterative_power(int nb, int power)
{
	int	nb_ori;

	nb_ori = nb;
	if (power < 0)
		return (0);
	if (power == 0)
		return (1);
	while (--power)
		nb *= nb_ori;
	return (nb);
}
