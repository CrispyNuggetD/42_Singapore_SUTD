/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   projection.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 05:48:59 by hnah              #+#    #+#             */
/*   Updated: 2026/08/26 05:50:57 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROJECTION_H
# define PROJECTION_H

# include "fdf.h"

t_projected		project_model(t_point point, t_map *map,
					t_projection *projection);
t_projection	init_projection(t_map *map);

#endif
