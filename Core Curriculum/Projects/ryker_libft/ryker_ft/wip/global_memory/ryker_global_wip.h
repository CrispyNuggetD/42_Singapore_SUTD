/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ryker_global_wip.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 14:22:16 by hnah              #+#    #+#             */
/*   Updated: 2026/09/17 14:22:16 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RYKER_GLOBAL_WIP_H
# define RYKER_GLOBAL_WIP_H

# include <stddef.h>
# include <stdlib.h>

typedef struct s_ryker_global
{
	void	**stuff;
	size_t	count;
}	t_ryker_global;

int				ryker_ft_global_append(t_ryker_global *container, void *payload);
t_ryker_global	*ryker_ft_global_container(void);
void			ryker_ft_global_clear(t_ryker_global *container);

#endif
