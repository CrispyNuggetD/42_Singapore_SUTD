/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   greedy_branch.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 15:56:51 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 17:49:18 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"

/* Keep the current insertion at [0]; append the child's winning plans. */
static void	append_child_path(t_greedy_path *candidate_path,
	const t_greedy_path *child_path)
{
	int	i;

	candidate_path->length = 1 + child_path->length;
	i = 0;
	while (i < child_path->length)
	{
		ft_memcpy(&candidate_path->plans[i + 1], &child_path->plans[i],
			sizeof(candidate_path->plans[i + 1]));
		i++;
	}
}

/*
** One branch owns these copies. The parent stacks never change.
** The child call finishes and returns a cost before this call continues.
** NULL disables recording, but executes the real rotation/push operations.
** At depth one, skip simulation unless the push empties B: alignment matters.
** A successful child search supplies the continuation to append after [0].
*/
static int	continuation_cost(t_circle_buf *a, t_circle_buf *b,
	t_greedy_search search, t_greedy_path *candidate_path)
{
	t_circle_buf	copies[2];
	t_greedy_path	child_path;
	int				remaining;

	if (search.depth == 1 && cbuf_len(b) > 1)
	{
		debug_lookahead_stop(0, 0, "depth limit");
		return (0);
	}
	ft_memcpy(&copies[A], a, sizeof(copies[A]));
	ft_memcpy(&copies[B], b, sizeof(copies[B]));
	if (greedy_execute_plan(NULL, &copies[A], &copies[B],
			&candidate_path->plans[0]) == ERROR)
		return (-1);
	search.depth--;
	search.budget -= candidate_path->plans[0].cost;
	search.best_path = &child_path;
	remaining = greedy_lookahead_cost(&copies[A], &copies[B], search);
	if (remaining >= 0)
		append_child_path(candidate_path, &child_path);
	return (remaining);
}

/* Each remaining insertion needs at least one pa. Equality cannot win ties. */
int	greedy_branch_cost(t_circle_buf *a, t_circle_buf *b,
	t_greedy_search search, t_greedy_path *candidate_path)
{
	int	min_remaining;
	int	remaining;

	debug_lookahead_try(search.depth, cbuf_len(b), &candidate_path->plans[0]);
	min_remaining = search.depth - 1;
	ryker_ft_update_min(&min_remaining, cbuf_len(b) - 1);
	if (candidate_path->plans[0].cost + min_remaining >= search.budget)
	{
		debug_lookahead_pruned(search.depth, cbuf_len(b));
		return (GREEDY_PRUNED);
	}
	remaining = continuation_cost(a, b, search, candidate_path);
	if (remaining < 0)
		return (remaining);
	return (candidate_path->plans[0].cost + remaining);
}
