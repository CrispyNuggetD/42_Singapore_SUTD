/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 22:59:37 by hnah              #+#    #+#             */
/*   Updated: 2026/10/24 23:02:38 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/*
** TODO: parent waits for child outcomes, not direct hunger checks.
** CHILD_DONE: keep waiting. CHILD_DIED: return STATUS_STOPPED for main cleanup.
** Child/wait failures: return an ERR_* so main stops and reaps the others.
** Hunger detection belongs to a monitoring thread inside each child process.
*/
t_status	supervise_children(t_sim_bonus *sim)
{
	(void)sim;
	return (ERR_NOT_IMPLEMENTED);
}
