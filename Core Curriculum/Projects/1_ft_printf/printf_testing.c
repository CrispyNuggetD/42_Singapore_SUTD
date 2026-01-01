/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 13:19:21 by hnah              #+#    #+#             */
/*   Updated: 2025/12/26 11:18:32 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "ft_printf.h>
#include <unistd.h>
#include <stdio.h>

// Let main stay on top (readability) with prototypes
//static t_handler	init_get_handlers(unsigned char fn_keys);
//static void			dispatch_key(unsigned char key);
//int					ft_printf(const char *key, ...);

// Main function
int	main(void)
{
	int test = (0 == -0); // passing -0 is same as passing 0 (NULL)
	
	printf("%i\n",,); // compiler error
	printf("%i\n",test);  // NORMAL print
	printf("%s","huh\n");  // NORMAL print
	//printf(%s);  // Does not allow/ compile fail
	printf(NULL);  // Gives warning, prints nothing
	printf("NULL");  // NORMAL print
	printf("");  // Gives warning, prints nothing
	printf("\nTesT3","1");  // Gives warning, Ignores second argument
	printf("\nTesT2%");  // Gives warning, Ignores % char
	printf("\nTesT1%s");  // Gives warning, prints crap
	//printf("\nTesT0%s",1);  // Gives warning, seg faults
	//printf(32883443499992);  // NORMAL print
	//printf("%s");  // Warning, seg faults
	//len = ft_printf("i");
}
// printf("Test ft_itoa(31)\nResult is: %s", ft_itoa(31));
