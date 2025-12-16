/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 15:07:30 by hnah              #+#    #+#             */
/*   Updated: 2025/12/16 12:55:49 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_lstclear_and_null(t_list **lst, void (*del)(void *));

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*result;
	t_list	*new;
	void	*new_content;

	result = NULL;
	if (!f || !del || !lst)
		return (NULL);
	while (lst)
	{
		new_content = f(lst -> content);
		if (!new_content)
			return (ft_lstclear_and_null(&result, del));
		new = ft_lstnew(new_content);
		if (!new)
		{
			del(new_content);
			return (ft_lstclear_and_null(&result, del));
		}
		ft_lstadd_back(&result, new);
		lst = lst -> next;
	}
	return (result);
}

void	*ft_lstclear_and_null(t_list **lst, void (*del)(void *))
{
	ft_lstclear(lst, del);
	return (NULL);
}
