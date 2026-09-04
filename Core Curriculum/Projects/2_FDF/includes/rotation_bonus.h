/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rotation_bonus.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 00:00:00 by hnah              #+#    #+#             */
/*   Updated: 2026/09/04 00:00:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROTATION_BONUS_H
# define ROTATION_BONUS_H

# include "fdf.h"

# define EVENT_KEY_RELEASE 3
# define MASK_KEY_RELEASE 2
# define FRAME_US 16667
# define ROTATION_SPEED 1.2
# ifdef __APPLE__
#  define KEY_A 0
#  define KEY_D 2
#  define KEY_LEFT 123
#  define KEY_RIGHT 124
# else
#  define KEY_A 97
#  define KEY_D 100
#  define KEY_LEFT 65361
#  define KEY_RIGHT 65363
# endif

typedef struct s_rotation
{
	t_info	info;
	double	angle;
	int		direction;
	long	last_frame;
}			t_rotation;

long			rotation_time_us(void);
t_projection	rotation_projection(t_rotation *rotation);
t_point			rotation_point(t_point point, t_rotation *rotation,
					t_projection *projection);
void			rotation_render(t_rotation *rotation);
int				rotation_key_press(int keycode, void *parameter);
int				rotation_key_release(int keycode, void *parameter);
int				rotation_loop(void *parameter);
int				rotation_close(void *parameter);

#endif
