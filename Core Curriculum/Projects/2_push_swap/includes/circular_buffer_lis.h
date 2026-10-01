/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   circular_buffer_lis.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 14:49:00 by hnah              #+#    #+#             */
/*   Updated: 2026/10/01 18:53:28 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CIRCULAR_BUFFER_LIS_H
# define CIRCULAR_BUFFER_LIS_H

/* Array positions are relative to start, not physical buffer indices. */
typedef struct s_cbuf_lis
{
	int	length[500];
	int	previous[500];
	int	count;
	int	start;
	int	best_length;
}	t_cbuf_lis;

#endif
