/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   algorithm.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 19:50:04 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 19:50:04 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ALGORITHM_H
# define ALGORITHM_H

/* Preparation choices only. */
typedef enum e_seed_mode
{
	SEED_THREE,
	SEED_LIS,
	SEED_COUNT
}	t_seed_mode;

/* Complete candidates: seed plus insertion strategy. */
typedef enum e_algorithm
{
	ALGO_THREE_LOCAL,
	ALGO_LIS_LOCAL,
	ALGO_THREE_LOOKAHEAD,
	ALGO_LIS_LOOKAHEAD,
	ALGO_COUNT
}	t_algorithm;

typedef struct s_algo_config
{
	t_seed_mode	seed;
	int			depth;
	const char	*name;
}	t_algo_config;

/* depth zero selects local greedy; positive depth selects lookahead. */
const t_algo_config	*algorithm_config(t_algorithm algo);

#endif
