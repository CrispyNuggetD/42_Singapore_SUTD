/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ryker_global_container_wip.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 14:22:16 by hnah              #+#    #+#             */
/*   Updated: 2026/09/17 14:22:16 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ryker_global_wip.h"

/* WIP: deliberately preserves the hidden shared-state idea for study. */
t_ryker_global	*ryker_ft_global_container(void)
{
	static t_ryker_global	container;

	return (&container);
}

/* Free only the pointer array: payload ownership stays with the caller. */
void	ryker_ft_global_clear(t_ryker_global *container)
{
	if (!container)
		return ;
	free(container->stuff);
	container->stuff = NULL;
	container->count = 0;
}
