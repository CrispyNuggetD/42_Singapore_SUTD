/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   greedy_stages.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 18:03:22 by hnah              #+#    #+#             */
/*   Updated: 2026/09/23 22:01:22 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"

/*
** TODO: Baseline preparation starts with empty B; leave at most three in A
** and sort that seed. Handle zero, one and two elements too.
** Postcondition: A is empty or circularly ascending; all ranks are retained.
** Keep this independent of insertion, so a future LIS/BFS seed can replace it.
** Existing small-sort helper is static in solve.c; inspect it before reuse.
*/
int	greedy_prepare(soln *x, circle_buf *a, circle_buf *b)
{
	(void)x;
	(void)a;
	(void)b;
	return (ERROR);
}

/*
** TODO: Repeatedly choose and execute a fresh plan until B is empty.
** Already empty B is SUCCESS. Do not prepare A here or reuse stale plans.
** Caller supplies empty or circularly ascending A, not a hidden unsorted tail.
*/
int	greedy_insert_all(soln *x, circle_buf *a, circle_buf *b)
{
	(void)x;
	(void)a;
	(void)b;
	return (ERROR);
}

/*
** TODO: Bring A's minimum to its top by the cheaper rotation direction.
** Requires circularly ascending A. Empty/singleton/already aligned: SUCCESS.
*/
int	greedy_align_min(soln *x, circle_buf *a)
{
	(void)x;
	(void)a;
	return (ERROR);
}

/*
** TODO: Compose preparation, insertion and alignment; propagate failures.
** Initial contract: B empty, distinct normalised ranks stored in A.
** After implementation, select this in solve.c when ready to try it.
** Future seed strategies can instead call insert_all and align_min directly.
*/
int	greedy_reinsertion(soln *x, circle_buf *a, circle_buf *b)
{
	(void)x;
	(void)a;
	(void)b;
	return (ERROR);
}
