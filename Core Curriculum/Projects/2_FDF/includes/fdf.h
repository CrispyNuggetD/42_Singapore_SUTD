/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fdf.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 03:27:00 by hnah              #+#    #+#             */
/*   Updated: 2026/08/26 00:43:39 by hnah             ###   ########.fr       */
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
	int		bytes_per_pixel;
	int		line_length;
	int		endian;
}		t_image;

typedef struct s_point
{
	int	x;
	int	y;
	int	z;
}		t_point;

typedef struct s_map
{
	int			width;
	int			height;
	t_point		*points;
}				t_map;

typedef struct s_info
{
	void		*mlx;
	void		*win;
	t_image		image;
	t_map		map;
}		t_info;

int		fdf_init(t_info *info, char *title);
int		fdf_close(t_info *info);
int		fdf_key_press(int keycode, t_info *info);
void	fdf_destroy(t_info *info);
void	fdf_put_pixel(t_image *image, int x, int y, unsigned int colour);
void	free_words(char **words);
int		map_dimensions(const char *filename, t_map *map);

#endif
