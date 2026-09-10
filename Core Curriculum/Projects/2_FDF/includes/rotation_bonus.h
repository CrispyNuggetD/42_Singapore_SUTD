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
# define PLAYER_SIZE 6
# define HOST_PLAYER 0
# define REMOTE_PLAYER 1
# define HOST_COLOUR 0x0000FF00
# define REMOTE_COLOUR 0x00FF0000
# define CAMERA_EYE_HEIGHT 2.0
# define CAMERA_FOV 1.0471975511965976
# define CAMERA_NEAR 0.1
# define REMOTE_TURN_STEP 0.15
# ifdef __APPLE__
#  define KEY_A 0
#  define KEY_D 2
#  define KEY_S 1
#  define KEY_V 9
#  define KEY_W 13
#  define KEY_LEFT 123
#  define KEY_RIGHT 124
# else
#  define KEY_A 97
#  define KEY_D 100
#  define KEY_S 115
#  define KEY_V 118
#  define KEY_W 119
#  define KEY_LEFT 65361
#  define KEY_RIGHT 65363
# endif

typedef struct s_player
{
	int	x;
	int	y;
}			t_player;

typedef enum e_view_mode
{
	VIEW_ISOMETRIC,
	VIEW_FIRST_PERSON
}			t_view_mode;

typedef struct s_camera
{
	double	x;
	double	y;
	double	z;
	double	yaw;
	double	focal;
	double	near_plane;
}			t_camera;

typedef struct s_camera_point
{
	double	right;
	double	vertical;
	double	depth;
	int		colour;
}			t_camera_point;

typedef struct s_viewport
{
	void	*win;
	t_image	image;
}			t_viewport;

typedef struct s_render_view
{
	t_camera	*camera;
	t_image		*image;
	int			player;
}			t_render_view;

typedef struct s_rotation
{
	t_info		info;
	t_player	players[2];
	t_camera	cameras[2];
	t_viewport	remote_view;
	t_view_mode	view_mode;
	double		angle;
	int			direction;
	int			remote_active;
	long		last_frame;
}			t_rotation;

long			rotation_time_us(void);
void			game_render(t_rotation *rotation);
void			game_update_camera(t_rotation *rotation, int player);
void			game_enable_first_person(t_rotation *rotation);
void			game_destroy(t_rotation *rotation);
int				game_close(void *parameter);
void			perspective_render(t_rotation *rotation, int player,
					t_image *image, void *win);
int				game_input_update(t_rotation *rotation);
int				game_move_player(t_rotation *rotation, int player, char key);
int				rotation_key_press(int keycode, void *parameter);
int				rotation_key_release(int keycode, void *parameter);
int				rotation_loop(void *parameter);

#endif
