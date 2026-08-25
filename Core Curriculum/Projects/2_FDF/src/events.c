/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   events.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 03:27:00 by hnah              #+#    #+#             */
/*   Updated: 2026/08/25 22:59:49 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

int	fdf_close(t_info *info)
{
	fdf_destroy(info);
	exit(0);
	return (0);
}

int	fdf_key_press(int keycode, t_info *info)
{
	if (keycode == KEY_ESC)
		fdf_close(info);
	return (0);
}
