/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   algorithm.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 19:50:04 by hnah              #+#    #+#             */
/*   Updated: 2026/09/30 20:09:48 by hnah             ###   ########.fr       */
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

/* BFS is first; greedy candidates run from ALGO_LIS_LOCAL to ALGO_COUNT. */
typedef enum e_algorithm
{
	ALGO_BFS,
	ALGO_LIS_LOCAL,
	ALGO_THREE_LOCAL,
	ALGO_LIS_LOOKAHEAD,
	ALGO_LIS_OPENING_ONE,
	ALGO_LIS_OPENING_BATCH,
	ALGO_COUNT
}	t_algorithm;

typedef struct s_algo_config
{
	t_seed_mode	seed; /* SEED_COUNT means no seed preparation (BFS). */
	int			use_lookahead;
	const char	*name;
}	t_algo_config;

/* Continuation: 0 = local, 1 = lookahead. NULL on invalid ID or settings. */
const t_algo_config	*algorithm_config(t_algorithm algo);

#endif
