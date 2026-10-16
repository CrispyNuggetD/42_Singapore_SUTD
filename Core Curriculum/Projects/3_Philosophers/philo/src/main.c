/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:14:55 by hnah              #+#    #+#             */
/*   Updated: 2026/10/12 22:15:33 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	main(int argc, char **argv)
{
	t_sim		sim;
	t_status	status;

	memset(&sim, 0, sizeof(sim));
	status = parse_input(argc, argv, &sim.config);
	if (status == STATUS_OK)
		status = init_sim(&sim);
	if (status == STATUS_OK)
		status = start_sim(&sim);
	return (cleanup_resources(&sim.resources, &sim.runtime, status));
}
