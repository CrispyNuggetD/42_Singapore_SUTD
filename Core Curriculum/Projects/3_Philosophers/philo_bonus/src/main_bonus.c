/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 20:55:17 by hnah              #+#    #+#             */
/*   Updated: 2026/11/02 10:53:22 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/*
** Initialize and clean up resources, including after partial setup failure.
** TODO: replace ERR_NOT_IMPLEMENTED with child startup and supervision.
*/
int	main(int argc, char **argv)
{
	t_philo_bonus	philo;
	t_status		status;

	memset(&sim, 0, sizeof(sim));
	status = parse_input(argc, argv, &sim.config);
	if (status == STATUS_OK)
		status = init_bonus(&sim);
	if (status == STATUS_OK)
		status = start_bonus(&sim);
	status = cleanup_bonus(&sim, status);
	return (print_return_error(status) > STATUS_STOPPED);
}
