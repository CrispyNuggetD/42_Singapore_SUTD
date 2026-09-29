/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_search.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 18:52:39 by hnah              #+#    #+#             */
/*   Updated: 2026/09/23 18:05:28 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBUG_SEARCH_H
# define DEBUG_SEARCH_H

typedef struct s_status_line
{
	char	text[256];
	int		length;
}	t_status_line;

/* Diagnostic counters only; never used to choose or execute a plan. */
typedef struct s_search_debug
{
	int					root_depth;
	int					depth_limit;
	int					initial_b;
	int					candidate[501];
	int					capped;
	int					active;
	int					percent;
	int					best_index;
	int					best_total;
	int					status_level;
	const char			*algo_label;
	int					algo_id;
	int					pass_active;
	int					pass_capped;
	unsigned long long	pass_total;
	unsigned long long	pass_done;
	unsigned long long	threshold[101];
	unsigned long long	total;
	unsigned long long	done;
}	t_search_debug;

t_search_debug	*debug_search_state(void);
void			debug_search_start(int b_len, int depth);
void			debug_pass_start(int b_len, int depth, int algo);
void			debug_search_reset(t_search_debug *s, int b_len, int depth);
void			debug_count_trials(t_search_debug *s);
void			debug_search_prefix(const char *event, int depth);
void			debug_search_progress(void);
void			debug_status_end(void);
void			debug_status_prepare(t_search_debug *s);
void			debug_status_draw(int complete);

#endif
