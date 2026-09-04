/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_bonus.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 00:00:00 by hnah              #+#    #+#             */
/*   Updated: 2026/09/04 00:00:00 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GAME_BONUS_H
# define GAME_BONUS_H

# include <sys/types.h>

# define GAME_INPUT_ERROR -1
# define GAME_INPUT_NONE 0
# define GAME_INPUT_KEY 1
# define GAME_INPUT_CLOSED 2

typedef struct s_game_input
{
	int	fd;
	int	original_flags;
	int	active;
}			t_game_input;

int	game_input_init(t_game_input *input, int fd);
int	game_input_read_key(t_game_input *input, char *key);
int	game_input_restore(t_game_input *input);
int	game_input_hook(void *parameter);

#endif
