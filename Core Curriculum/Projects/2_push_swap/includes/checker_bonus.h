/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker_bonus.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/02 01:13:54 by hnah              #+#    #+#             */
/*   Updated: 2026/10/02 02:42:29 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHECKER_BONUS_H
# define CHECKER_BONUS_H

# include "push_swap.h"

/* Pointer to a move function taking a solution and one or two stack(s). */
typedef int	(*t_single_move)(t_soln *, t_circle_buf *);
typedef int	(*t_double_move)(t_soln *, t_circle_buf *, t_circle_buf *);

int		checker_read_apply_moves(t_circle_buf stacks[2]);
int		checker_apply_move(const char *line, t_circle_buf stacks[2]);

#endif
