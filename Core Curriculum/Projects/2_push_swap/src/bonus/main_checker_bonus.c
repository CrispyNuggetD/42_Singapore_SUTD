/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_checker_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/02 01:13:29 by hnah              #+#    #+#             */
/*   Updated: 2026/10/02 02:36:55 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "checker_bonus.h"

/*
 * No arguments means nothing to check, so exit without reading stdin.
 * Validate and rank the input, then apply stdin instructions until EOF.
 * Valid execution prints OK only when A is sorted and B is empty; otherwise KO.
 * Parsing or instruction errors print Error to stderr instead of a verdict.
 * Free both optional heap buffers, including after partial setup failures.
 */
int	main(int argc, char **argv)
{
	t_circle_buf	stacks[2];
	int				status;

	if (argc == 1)
		return (SUCCESS);
	stacks[A].large_buf = NULL;
	stacks[B].large_buf = NULL;
	status = parse_input(argv + 1, &stacks[A], &stacks[B]);
	if (status == SUCCESS)
		status = checker_read_apply_moves(stacks);
	if (status == SUCCESS)
	{
		if (cbuf_is_empty(&stacks[B]) && ranks_are_sorted(&stacks[A]))
			ft_putendl_fd("OK", STDOUT_FILENO);
		else
			ft_putendl_fd("KO", STDOUT_FILENO);
	}
	free(stacks[A].large_buf);
	free(stacks[B].large_buf);
	if (status == ERROR)
		ft_putendl_fd("Error", STDERR_FILENO);
	return (status);
}
