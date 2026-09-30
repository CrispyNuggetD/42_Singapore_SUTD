/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bfs_seed.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:11:30 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 01:23:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BFS_SEED_H
# define BFS_SEED_H

# include "push_swap.h"

typedef struct s_seed_search
{
	t_brutenode		*nodes;
	unsigned char	*visited;
	int				n;
	int				capacity;
	int				head;
	int				count;
}	t_seed_search;

int	bfs_seed_sort(soln *x, circle_buf *a, circle_buf *b);
int	seed_search_init(t_seed_search *search, circle_buf *a);
int	seed_search_goal(t_seed_search *search);
int	seed_is_goal(t_brutestate *state, int n);
int	seed_replay(soln *x, circle_buf *a, circle_buf *b, char *route);
#endif
