/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker_bonus.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/02 01:13:54 by hnah              #+#    #+#             */
/*   Updated: 2026/10/02 01:13:55 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHECKER_BONUS_H
# define CHECKER_BONUS_H

# include "push_swap.h"

/* SUCCESS after EOF; ERROR on invalid instructions or a read failure. */
int		checker_read_moves(t_circle_buf stacks[2]);
/* Match the entire line including '\n'; use NULL as the solution pointer. */
/* pa/pb on an empty source are valid no-ops; guard before calling wrappers. */
int		checker_apply_move(const char *line, t_circle_buf stacks[2]);
/* True only when A is ascending and B is empty; values are normalised ranks. */
int		checker_is_sorted(t_circle_buf stacks[2]);
/* Free both large_buf pointers; safe after initialising them to NULL. */
void	checker_cleanup(t_circle_buf stacks[2]);

#endif
