/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ultimate_div_mod.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 17:06:30 by hnah              #+#    #+#             */
/*   Updated: 2025/07/30 17:20:56 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <stdio.h>

void	ft_ultimate_div_mod(int *a, int *b)
{
	int	div;
	int	mod;

	div = (*a / *b);
	mod = (*a % *b);
	*a = div;
	*b = mod;
}

/*int	main(void)
{
	int	div;
	int	mod;

	div = 17;
	mod = 10;
	ft_ultimate_div_mod(&div, &mod);
	printf("%d,%d", div, mod);
}*/
