/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 16:36:13 by hnah              #+#    #+#             */
/*   Updated: 2025/12/11 19:07:35 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	ab;
	size_t	xy;

	if (!*little)
		return ((char *)big);
	ab = 0;
	while (big[ab] && ab < len)
	{
		xy = 0;
		while (big[ab + xy] && little [xy]
			&& ab + xy < len && big[ab + xy] == little[xy])
		{
			xy++;
			if (!little[xy])
				return ((char *)(big + ab));
		}
		ab++;
	}
	return (NULL);
}

/* pseudocode used. Idk why it was in Rust.
 *
 * this function took so ridiculously long to think of how to do.. grrr..
 * #never_give_up
 *
 * Anyway i'm really not used to indexing method, but here goes:
 * xy = 0 resets the needle (say failed check)
 *
 * Then loop first line: while (big[ab + xy] && little[xy]
 * Checks if the current big and the shorter little has null terminated
 *
 * then:
 * && ab + xy < len
 * Checks that this needle in a haystack doesn't > length allowance
 *
 * Then FINALLY:
 * big[ab + xy] == little[xy]
 * equivalence checker for searching to start.
 *
 * During searching, ab is at the start of the search in big.
 * If a true needle is found, needle will null terminate,
 * hence we return const original big ptr + ab moved.
 *
 * move big index ab if equivalence broke and not true complete needle.
 *
 * *SOBS*
 */
