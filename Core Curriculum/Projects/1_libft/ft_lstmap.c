/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 15:07:30 by hnah              #+#    #+#             */
/*   Updated: 2025/12/15 16:42:57 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*result;
	t_list	*new;

	result = NULL;
	if (!f || !del || !lst)
		return (NULL);
	while (lst)
	{
		if (!f(lst -> content))
			del(lst -> content);
		new = ft_lstnew(f(lst -> content));
		ft_lstadd_back(&result, new);
		lst = lst -> next;
	}
	return (result);
}
