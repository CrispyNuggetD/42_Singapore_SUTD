/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fdf.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 03:27:00 by hnah              #+#    #+#             */
/*   Updated: 2026/08/25 22:59:49 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FDF_H
# define FDF_H

# include "mlx.h"
# include <stdlib.h>
# include <unistd.h>

# define WIN_WIDTH 1000
# define WIN_HEIGHT 700
# define KEY_ESC 65307
# define EVENT_KEY_PRESS 2
# define EVENT_DESTROY 17

typedef struct s_image
{
	void	*ptr;
	char	*addr;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
}		t_image;

typedef struct s_info
{
	void		*mlx;
	void		*win;
	t_image		image;
}		t_info;

int		fdf_init(t_info *info, char *title);
int		fdf_close(t_info *info);
int		fdf_key_press(int keycode, t_info *info);
void	fdf_destroy(t_info *info);

#endif
