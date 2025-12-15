/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstdelone.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 11:19:17 by hnah              #+#    #+#             */
/*   Updated: 2025/12/15 12:47:58 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstdelone(t_list *lst, void (*del)(void *))
{
	t_list	*lst_next;

	if (!del || !lst)
		return ;
	del(lst -> content);
	lst_next = lst -> next;
	if (lst_next)
	{
		ft_memcpy(lst, lst -> next, sizeof(t_list));
		free(lst_next);
		return ;
	}
	free(lst);
	return ;
}

/* I THOUGHT we had to relink this list and keep the new contents.
 * Meaning B -> A(del) but B.contents is not deleted (assumption = invalid).
 * 
 * But even by that thought:
 * free(lst_next -> next); and
 * free(lst_next -> content);
 * can't be run cos it'll free new lst's stuff.
 *
 * So it feels like just free the node struct and
 * the pointers will be auto-destroyed.
 *
 * HOWEVER:
 * Caller still has a pointer to the previous node that points to this
 * last node, what is prev->next after I free the list?
 * That's right: it's still pointing to freed memory (dangling).
 * So unless the caller unlinked first, I just created a use-after-free.
 *
 * Plus: If someone expected ft_lstdelone(lst, del) to destroy the node
 * lst points to, they will be surprised that the DATA from the next node
 * slid into that address.
 * That can break code that tracks node identity by address.
