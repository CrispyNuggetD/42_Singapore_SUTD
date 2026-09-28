/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs_seed_wip.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:11:30 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 01:23:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BFS_SEED_WIP_H
# define BFS_SEED_WIP_H

# include "push_swap.h"

# define WIP_SEED_SIZE 10

/* Distinguish an unfinished search from failure or a completed route. */
typedef enum e_wip_bfs_status
{
	WIP_BFS_FOUND,
	WIP_BFS_EXHAUSTED,
	WIP_BFS_LIMIT,
	WIP_BFS_ERROR,
	WIP_BFS_NOT_IMPLEMENTED
}	t_wip_bfs_status;

/* Limits apply to the search, not to the number of values tracked in B. */
typedef struct s_wip_bfs_limits
{
	int	max_nodes;
	int	max_depth;
}	t_wip_bfs_limits;

/* Draft representation: both real stacks, including all values in B. */
typedef struct s_wip_bfs_node
{
	circle_buf	stacks[2];
	int			parent;
	int			depth;
	char		move;
}	t_wip_bfs_node;

int					wip_bfs_seed_is_goal(circle_buf *a);
t_wip_bfs_status	wip_bfs_seed_search(soln *x, circle_buf *a,
						circle_buf *b, const t_wip_bfs_limits *limits);
#endif
