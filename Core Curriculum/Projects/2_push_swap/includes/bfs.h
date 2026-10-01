/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/10 20:28:42 by hnah              #+#    #+#             */
/*   Updated: 2026/10/01 18:16:06 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BFS_H
# define BFS_H

# include "push_swap.h"

/*
** Report progress every this many expanded nodes; must be positive.
** 32768 is 2^15, allowing the old bitmask check: (head & 32767) == 0.
** BFS does not require powers of two; modulo also accepts e.g. 10000.
** Smaller intervals refresh more often but add printing overhead.
*/
# define BFS_PROGRESS_INTERVAL 32768

/* Pending queue: nodes[head..total-1]. Earlier nodes retain parent links. */
typedef struct s_bfs_search
{
	t_brutenode		*nodes;
	unsigned char	*visited;
	int				count;
	int				capacity;
	int				head;
	int				total;
}	t_bfs_search;

/* Requires room for (len(A) + 1)! nodes, empty B and local ranks in A. */
int	bfs_find_goal(t_brutenode *nodes, t_circle_buf *a, t_circle_buf *b);

#endif
