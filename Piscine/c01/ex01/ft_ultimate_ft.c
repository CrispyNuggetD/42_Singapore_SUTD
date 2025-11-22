/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ultimate_ft.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 16:21:29 by hnah              #+#    #+#             */
/*   Updated: 2025/07/25 16:37:52 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <unistd.h>

void	ft_ultimate_ft(int *********nbr)
{
	*********nbr = 42;
}

/*int main(void)
{
	int nbr = 42;

	int *ptr1 = &nbr;
	int **ptr2 = &ptr1;
	int ***ptr3 = &ptr2;
	int ****ptr4 = &ptr3;
	int *****ptr5 = &ptr4;
	int ******ptr6 = &ptr5;
	int *******ptr7 = &ptr6;
	int ********ptr8 = &ptr7;
	int *********ptr9 = &ptr8;
	
	ft_ultimate_ft(ptr9);

	// Now dereference 9 times to get back to the int value
	char tens = (nbr / 10) + '0';
	char ones = (nbr % 10) + '0';

	write(1, &tens, 1);
	write(1, &ones, 1);

	return (0);
}*/
