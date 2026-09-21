/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ryker_global_append_wip.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 14:22:16 by hnah              #+#    #+#             */
/*   Updated: 2026/09/17 14:22:16 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ryker_global_wip.h"

/* WIP: stores borrowed pointers, not copies of the pointed-to objects. */
int	ryker_ft_global_append(t_ryker_global *container, void *payload)
{
	void	**items;
	size_t	i;

	if (!container || !payload)
		return (0);
	if (container->count >= (size_t)-1 / sizeof(*items))
		return (0);
	items = malloc((container->count + 1) * sizeof(*items));
	if (!items)
		return (0);
	i = 0;
	while (i < container->count)
	{
		items[i] = container->stuff[i];
		i++;
	}
	items[i] = payload;
	free(container->stuff);
	container->stuff = items;
	container->count++;
	return (1);
}
