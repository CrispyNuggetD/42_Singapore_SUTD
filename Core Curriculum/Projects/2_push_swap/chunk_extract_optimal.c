#include "push_swap.h"
#include <limits.h>

#define DIR_RA 1
#define DIR_RRA -1

static int	is_target(int value, int min, int max)
{
	return (value >= min && value <= max);
}

static char	*direction_name(int direction)
{
	if (direction == DIR_RA)
		return ("RA");
	return ("RRA");
}

static void	set_initial_direction(int *direction, int first, int turn_after)
{
	*direction = first;
	if (turn_after == 0)
		*direction = -*direction;
}

static int	simulate_route(cbuf *a, int min, int max, int first,
		int turn_after)
{
	cbuf	temp;
	int		direction;
	int		collected;
	int		cost;
	int		popped;

	temp = *a;
	set_initial_direction(&direction, first, turn_after);
	collected = 0;
	cost = 0;
	while (collected < max - min + 1)
	{
		while (!is_target(temp.buf[temp.read_idx], min, max))
		{
			if (direction == DIR_RA)
				cbuf_rotate(&temp);
			else
				cbuf_rev_rotate(&temp);
			cost++;
		}
		if (cbuf_pop_top(&temp, &popped) == ERROR)
			return (INT_MAX);
		cost++;
		collected++;
		if (collected == turn_after)
			direction = -direction;
	}
	return (cost);
}

static int	execute_route(soln *x, cbuf *a, cbuf *b, int min, int max,
		int first, int turn_after)
{
	int	direction;
	int	collected;

	set_initial_direction(&direction, first, turn_after);
	collected = 0;
	while (collected < max - min + 1)
	{
		while (!is_target(a->buf[a->read_idx], min, max))
		{
			if (direction == DIR_RA && ra(x, a) == ERROR)
				return (ERROR);
			if (direction == DIR_RRA && rra(x, a) == ERROR)
				return (ERROR);
		}
		if (pb(x, a, b) == ERROR)
			return (ERROR);
		collected++;
		if (collected == turn_after)
			direction = -direction;
	}
	return (SUCCESS);
}

int	extract_chunk_optimal(soln *x, cbuf *a, cbuf *b, int min, int max)
{
	int	best_cost;
	int	best_direction;
	int	best_turn_after;
	int	first;
	int	turn_after;
	int	cost;

	best_cost = INT_MAX;
	best_direction = DIR_RA;
	best_turn_after = 0;
	first = DIR_RA;
	while (first == DIR_RA || first == DIR_RRA)
	{
		turn_after = 0;
		while (turn_after <= max - min + 1)
		{
			cost = simulate_route(a, min, max, first, turn_after);
			if (cost < best_cost)
			{
				best_cost = cost;
				best_direction = first;
				best_turn_after = turn_after;
			}
			turn_after++;
		}
		if (first == DIR_RA)
			first = DIR_RRA;
		else
			break ;
	}
	printf("CHUNK %d..%d ROUTE cost=%d start=%s turn_after=%d\n", min, max,
		best_cost, direction_name(best_direction), best_turn_after);
	return (execute_route(x, a, b, min, max, best_direction,
			best_turn_after));
}
