/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/14 19:12:11 by hnah              #+#    #+#             */
/*   Updated: 2025/12/15 11:18:20 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*moving;

	if (!new)
		return ;
	if (!*lst)
	{
		*lst = new;
		return ;
	}
	moving = *lst;
	while (moving -> next)
		moving = moving -> next;
	moving -> next = new;
	return ;
}

/* 
 * Apparently that's the cursor moving style which is a horrid to do/ explain
 * anyway i found out later there's an elegant slot-walk technique
 * that should be used for insertion/ deletion (former is transversal).
 *
 * No need to think of NULL list edge case.
 * Both t_list *add, but former is a head pointer and
 * this is the &NEXT itself.
 *
 * So this will suffice to pass this project, lol:
 * 	
 * 	t_list **cur = lst;
 * 	while (*cur)
 * 		cur = &((*cur) -> next);
 * 	*cur = new;
 */


