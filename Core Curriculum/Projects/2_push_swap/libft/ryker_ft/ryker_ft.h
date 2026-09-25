/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ryker_ft.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 05:27:07 by hnah              #+#    #+#             */
/*   Updated: 2026/09/14 03:44:33 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RYKER_FT_H
# define RYKER_FT_H

# include "printf_fd/ryker_ft_printf.h"
# include "gnl_status/ryker_ft_get_next_line.h"
# include "file_unique/ryker_ft_file_unique.h"

int	ryker_ft_isspace(int c);

int		ryker_ft_max(int one, int two);

/* Precondition: one != INT_MIN. */
int		ryker_ft_abs(int one);

/* Return -1 for NULL, 0 unchanged, 1 updated. dest must be initialised. */
int		ryker_ft_update_max(int *dest, int candidate);
int		ryker_ft_update_min(int *dest, int candidate);

int		ryker_ft_sign(int value);

#endif
