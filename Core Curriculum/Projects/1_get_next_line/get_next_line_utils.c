/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   get_next_line_utils.c							  :+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: hnah <hnah@student.42singapore.sg>		 +#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2025/12/25 13:19:21 by hnah			  #+#	#+#			 */
/*   Updated: 2026/01/09 18:55:24 by hnah			 ###   ########.fr	   */
/*																			*/
/* ************************************************************************** */

#include "get_next_line.h"
//#include <stdio.h>

// Let main stay on top (readability) with prototypes
static t_handler	init_get_handlers(unsigned char fn_keys);

// Main function
/* int	main(void)
{
	int	mylen;
	int	printf_len;

	printf(" ===== MANDATORY BELOW ===== \n");
	printf("\n ===== STRINGS ===== \n");
	ft_printf("My output is:\n");
	mylen = ft_printf(" %s 123aa ", "test");
	printf("\nMy ft_printf return value:%i\n\n", mylen);

	ft_printf("Actual printf output is:\n");
	printf_len = printf(" %s 123aa ", "test");
	printf("\nActual printf return value:%i\n\n", printf_len);
} */


