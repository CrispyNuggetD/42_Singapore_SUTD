/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_push_swap.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:41:55 by hnah              #+#    #+#             */
/*   Updated: 2026/05/05 18:34:17 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	main(int argc, char **argv)
{
	int	*yoink_number = NULL;
	int	temp_error_clean_up_placeholder = 0;
	if (argc != 2 || argv == NULL)
		return (ERR_INVALID_INPUT);
	else
	{
		if (parse_input(argv) == ERROR)
			return (ERR_PARSE_INPUT);
		
	}
	return (SUCCESS);
}