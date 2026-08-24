/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fdf.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 00:00:00 by hnah              #+#    #+#             */
/*   Updated: 2026/08/25 00:00:00 by hnah             ###   ########.fr       */
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

typedef struct s_fdf
{
	void		*mlx;
	void		*win;
	t_image		image;
}		t_fdf;

int		fdf_init(t_fdf *fdf, char *title);
int		fdf_close(t_fdf *fdf);
int		fdf_key_press(int keycode, t_fdf *fdf);
void	fdf_destroy(t_fdf *fdf);

#endif
