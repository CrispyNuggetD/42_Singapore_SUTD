/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_combn.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 17:53:50 by hnah              #+#    #+#             */
/*   Updated: 2025/07/27 12:13:50 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

/* This is an insane attempt at recursion
 * The recursion helper function ("recursion") takes
 * 4 arguments that are required so that each 
 * recursion remembers the previous value.
 *
 * recursion(int pos, int n, char *buffer, int prev)
 * is the helper function.
 *
 * Inside the helper function, there is a local
 * variable "digit" to manipulate to store the
 * ascended digt.
 *
 * =============
 *
 * Of the arguments:
 * pos remembers the temporary index where the
 * number exists inside the character array buffer[10].
 *
 * buffer[10] is a temporary array to loop through
 * 0-9 for every single time it tries to print
 * out an ascending digit. The values inside gets 
 * recycled over time.
 *
 * prev stores the previous value so that the next
 * recursion can compare the current digit to ensure
 * the current digit is "higher" than the previous digit
 * 
 * ==============
 *
 * First, the recursion is ran by:
 * recursion(0, n, buffer, -1)

 * The first portion of "recursion" first checks
 * if (pos == n). It is a "trigger"
 * that when the pos (as it increments during the recursion) 
 * reached the number of digits required,
 * that it starts outputing the digits.
 *
 * Otherwise, it loops and first sets the current run's
 * variable "digit" to be 1 more than the previous ("prev")
 * value.
 *
 * The loop runs from 0 to 9 (the digits) and increments
 * by 1 each time.
 *
 * At index "pos" of the buffer array (first run is 0),
 * it stores the ASCII digit to be printed later.
 *
 * It then recurs by calling recursion again,
 * this time pos+1, and using digit as the "prev" digit
 * (hence memory).
 *
 * The recursion stores the new "digit" (prev digit + 1)
 * in the next index (pos+1).
 *
 * And again, when we hit the trigger, say the exercise
 * expects length 2 (n=2),
 * Then it will write "buffer" to the output 
 * using write(1, buffer, n).
 *
 * The last part of the trigger checks that the first
 * index of the buffer is not ('9' - n + 1).
 *
 * For example, if n = 2, the last valid output 
 * is "89". ('9' - n + 1) is '8'. Hence, when that happens,
 * the final number will not have a ", " appended to it,
 * ending the output.
 *
 * However, there is a special case: when n = 9,
 * ('9' - 9 + 1 = '1'), but buffer[0] is 0,
 * since there is only one case where 
* buffer[0] is not 0 (the last case), "123456789".
*
* Hence, to check for both conditions, we use:
* if (!(pos == n && buffer[0] == '9' - n + 1)),
	* where we check the condition that it is NOT the 
	* case where pos is n and the first digit is ('9' - n + 1).
	*
	* The only case where this condition is true is when 
	* n = 9 as pos = 9 could happen, making the AND
	* statement true, hence a logical NOT makes it
	* NOT print the ", ".
*/

void	recursion(int pos, int n, char *buffer, int prev)
{
	int	digit;

	if (pos == n)
	{
		write(1, buffer, n);
		if (buffer[0] != '9' - n + 1)
			write(1, ", ", 2);
		return ;
	}
	digit = prev + 1;
	while (digit <= 9)
	{
		buffer[pos] = digit + '0';
		recursion(pos + 1, n, buffer, digit);
		digit++;
	}
}

void	ft_print_combn(int n)
{
	char	buffer[10];

	recursion(0, n, buffer, -1);
}
