/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   usage_printer.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 10:10:34 by hnah              #+#    #+#             */
/*   Updated: 2026/09/17 14:40:19 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	print_usage(t_usage_message_id message_id)
{
	if (message_id == USAGE_MANDATORY)
		ryker_ft_printf_fd(2, "Usage: ./pipex infile cmd1 cmd2 outfile\n");
	else if (message_id == USAGE_HEREDOC)
		ryker_ft_printf_fd(2,
			"Usage: ./pipex here_doc LIMITER cmd... outfile\n");
	else if (message_id == USAGE_BONUS)
		ryker_ft_printf_fd(2, "Usage: ./pipex infile cmd... outfile\n");
	return (1);
}
