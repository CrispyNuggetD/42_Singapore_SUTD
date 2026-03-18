/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 14:33:08 by hnah              #+#    #+#             */
/*   Updated: 2025/12/15 15:00:21 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*moving;
	t_list	*prev_node;

	if (lst == NULL)
		return ;
	moving = *lst;
	if (moving == NULL)
	{
		lst = NULL;
		return ;
	}
	while (moving -> next)
	{
		del(moving -> content);
		prev_node = moving;
		moving = moving -> next;
		free(prev_node);
	}
	del(moving -> content);
	free(moving);
	*lst = NULL;
	return ;
}
