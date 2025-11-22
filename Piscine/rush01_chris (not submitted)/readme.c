/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readme.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 19:27:32 by hnah              #+#    #+#             */
/*   Updated: 2025/08/03 19:27:34 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



/* This is copied and have not been rewritten into my own code
 * 
 * This is a possible algo. I devised with A.I. to generate permutations
 * that we could have used to do the following problem approach:
 *
 * === ELIMINATION METHOD ===
 * After argc argv reading and basic error handling,
 *
 * Main pruning for knowing tallest to eliminate from all rows/ cols
 * 1. Handle clue Input = 1
 * 2. Handle clue pair = (Sum of top and bottom) is n+1, highest known.
 *
 * Make a [4][4][4] (or n,n,n) array big enough to have the last dimension
 * hold whether it is that number as we loop through the code.
 * 
 * For instance, [0,0,0,1] means we know for certain that number is 4.
 * 
 * [0,1,1,0] means it could be 2 or 3 we still don’t know. 
 * And [0,0,0,0] means it can’t be any number, contradiction, error.
 *
 * We generate all possible permutations and store it in an array
 * 
 * We run a function over that array to see what’s the “height” from
 * the left and right side, and store it in another sorted array as a table to refer to.
 * 
 * Do that code there to go through every box every number and see if that
 * number exist in (point 4). 
 *
 * If it does not, set that number to be 0 in the [1,1,1,0] (e.g. for number 4).
 *
 * Loop until every box is done.
 */















// From ChatGPT


/*
// #include <stdlib.h>

int **g_perm_table;
int g_perm_count = 0;

void	swap(int *x, int *y)
{
	int tmp = *x;
	*x = *y;
	*y = tmp;
}

void	store_perm(int *perm, int size)
{
	for (int i = 0; i < size; i++)
		g_perm_table[g_perm_count][i] = perm[i];
	g_perm_count++;
}

void	generate_perms(int *perm, int size, int pos)
{
	if (pos == size)
	{
		store_perm(perm, size);
		return;
	}
	for (int i = pos; i < size; i++)
	{
		swap(&perm[pos], &perm[i]);
		generate_perms(perm, size, pos + 1);
		swap(&perm[pos], &perm[i]);
	}
}

void	init_permutations(int size)
{
	int *perm = malloc(sizeof(int) * size);
	if (!perm)
		return;

	for (int i = 0; i < size; i++)
		perm[i] = i + 1;

	generate_perms(perm, size, 0);
	free(perm);
}*/
