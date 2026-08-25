/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fdf.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 03:27:00 by hnah              #+#    #+#             */
/*   Updated: 2026/08/26 05:50:57 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FDF_H
# define FDF_H

# include "mlx.h"
# include <math.h>
# include <stdlib.h>
# include <unistd.h>

# define WIN_WIDTH 1000
# define WIN_HEIGHT 700
# define KEY_ESC 65307
# define EVENT_KEY_PRESS 2
# define EVENT_DESTROY 17
# define MASK_KEY_PRESS 1
# define PI 3.14159265358979323846
# define WINDOW_MARGIN 80

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

typedef struct s_projection
{
	double	cos_angle;
	double	sin_angle;
	double	scale;
	double	offset_x;
	double	offset_y;
}			t_projection;

typedef struct s_projected
{
	double	x;
	double	y;
}			t_projected;

typedef struct s_bounds
{
	double	min_x;
	double	max_x;
	double	min_y;
	double	max_y;
}			t_bounds;

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
void	draw_line(t_image *image, t_point start, t_point end);
void	free_words(char **words);
char	**split_map_line(char *line);
int		map_dimensions(const char *filename, t_map *map);
int		allocate_map(t_map *map);
int		read_map(const char *filename, t_map *map);
void	render_map(t_info *info);

#endif
