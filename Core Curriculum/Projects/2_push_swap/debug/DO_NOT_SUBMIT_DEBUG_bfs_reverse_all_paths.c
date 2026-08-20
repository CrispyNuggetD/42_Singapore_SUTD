/* ************************************************************************** */
/*                                                                            */
/*       DO NOT SUBMIT - REVERSE BFS / ALL SHORTEST SOLUTION PATHS           */
/*                                                                            */
/* ************************************************************************** */

/*
 * Builds the complete push_swap state graph once, backwards from the sorted
 * goal.  Each state is stored at its Lehmer/split ID, so no state is inserted
 * twice.  All shortest forward solutions are then the edges which reduce the
 * stored distance by exactly one.
 *
 * This debug program uses unauthorised functions.  Do not submit it.
 */

#include "../push_swap.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PATH_MAX_MOVES 128

typedef struct s_reverse_graph
{
	t_brutestate		*states;
	int				*distance;
	unsigned long long	*path_count;
	int				*queue;
	int				n;
	int				state_count;
	int				discovered;
	int				max_distance;
	int				count_overflow;
} t_reverse_graph;

static const char	g_moves[11] = {
	SA, SB, SS, PA, PB, RA, RB, RR, RRA, RRB, RRR
};

static int	factorial(int n)
{
	int	result;

	result = 1;
	while (n > 1)
		result *= n--;
	return (result);
}

static int	same_state(const t_brutestate *a, const t_brutestate *b, int n)
{
	int	i;

	if (a->split != b->split)
		return (0);
	i = 0;
	while (i < n)
	{
		if (a->value[i] != b->value[i])
			return (0);
		i++;
	}
	return (1);
}

static char	inverse_move(char move)
{
	if (move == PA)
		return (PB);
	if (move == PB)
		return (PA);
	if (move == RA)
		return (RRA);
	if (move == RB)
		return (RRB);
	if (move == RR)
		return (RRR);
	if (move == RRA)
		return (RA);
	if (move == RRB)
		return (RB);
	if (move == RRR)
		return (RR);
	return (move);
}

static int	is_real_predecessor(t_brutestate *predecessor,
		const t_brutestate *current, char forward_move, int n)
{
	t_brutestate	check;

	*predecessor = *current;
	brute_apply_move(predecessor, inverse_move(forward_move), n);
	check = *predecessor;
	brute_apply_move(&check, forward_move, n);
	return (same_state(&check, current, n));
}

static int	allocate_graph(t_reverse_graph *graph, int n)
{
	int	i;

	graph->n = n;
	graph->state_count = (n + 1) * factorial(n);
	graph->states = malloc(sizeof(*graph->states) * graph->state_count);
	graph->distance = malloc(sizeof(*graph->distance) * graph->state_count);
	graph->path_count = calloc((size_t)graph->state_count,
		 sizeof(*graph->path_count));
	graph->queue = malloc(sizeof(*graph->queue) * graph->state_count);
	if (graph->states == NULL || graph->distance == NULL
		|| graph->path_count == NULL || graph->queue == NULL)
		return (ERROR);
	i = 0;
	while (i < graph->state_count)
		graph->distance[i++] = -1;
	graph->discovered = 0;
	graph->max_distance = 0;
	graph->count_overflow = 0;
	return (SUCCESS);
}

static void	free_graph(t_reverse_graph *graph)
{
	free(graph->states);
	free(graph->distance);
	free(graph->path_count);
	free(graph->queue);
}

static void	make_goal(t_brutestate *goal, int n)
{
	int	i;

	goal->split = n;
	i = 0;
	while (i < n)
	{
		goal->value[i] = i;
		i++;
	}
}

static void	insert_predecessors(t_reverse_graph *graph, int current_id,
		int *tail)
{
	t_brutestate	predecessor;
	int			predecessor_id;
	int			move_index;

	move_index = 0;
	while (move_index < 11)
	{
		if (is_real_predecessor(&predecessor, &graph->states[current_id],
				g_moves[move_index], graph->n))
		{
			predecessor_id = calculate_state_id(&predecessor, graph->n);
			if (graph->distance[predecessor_id] == -1)
			{
				graph->states[predecessor_id] = predecessor;
				graph->distance[predecessor_id]
					= graph->distance[current_id] + 1;
				graph->queue[(*tail)++] = predecessor_id;
				graph->discovered++;
				if (graph->distance[predecessor_id] > graph->max_distance)
					graph->max_distance = graph->distance[predecessor_id];
			}
		}
		move_index++;
	}
}

static int	build_reverse_graph(t_reverse_graph *graph)
{
	t_brutestate	goal;
	int			goal_id;
	int			head;
	int			tail;

	make_goal(&goal, graph->n);
	goal_id = calculate_state_id(&goal, graph->n);
	graph->states[goal_id] = goal;
	graph->distance[goal_id] = 0;
	graph->queue[0] = goal_id;
	graph->discovered = 1;
	head = 0;
	tail = 1;
	while (head < tail)
		insert_predecessors(graph, graph->queue[head++], &tail);
	return (graph->discovered == graph->state_count ? SUCCESS : ERROR);
}

static unsigned long long	add_count(t_reverse_graph *graph,
		unsigned long long total, unsigned long long add)
{
	if (ULLONG_MAX - total < add)
	{
		graph->count_overflow = 1;
		return (ULLONG_MAX);
	}
	return (total + add);
}

static unsigned long long	count_paths(t_reverse_graph *graph, int state_id)
{
	t_brutestate			next;
	unsigned long long	total;
	int					next_id;
	int					move_index;

	if (graph->distance[state_id] == 0)
		return (1);
	if (graph->path_count[state_id] != 0)
		return (graph->path_count[state_id]);
	total = 0;
	move_index = 0;
	while (move_index < 11)
	{
		next = graph->states[state_id];
		brute_apply_move(&next, g_moves[move_index], graph->n);
		next_id = calculate_state_id(&next, graph->n);
		if (graph->distance[next_id] == graph->distance[state_id] - 1)
			total = add_count(graph, total, count_paths(graph, next_id));
		move_index++;
	}
	graph->path_count[state_id] = total;
	return (total);
}

static const char	*move_name(char move)
{
	if (move == SA)
		return ("sa");
	if (move == SB)
		return ("sb");
	if (move == SS)
		return ("ss");
	if (move == PA)
		return ("pa");
	if (move == PB)
		return ("pb");
	if (move == RA)
		return ("ra");
	if (move == RB)
		return ("rb");
	if (move == RR)
		return ("rr");
	if (move == RRA)
		return ("rra");
	if (move == RRB)
		return ("rrb");
	if (move == RRR)
		return ("rrr");
	return ("?");
}

static void	write_path(FILE *file, const char *path, int length)
{
	int	i;

	i = 0;
	while (i < length)
	{
		if (i > 0)
			fprintf(file, " ");
		fprintf(file, "%s", move_name(path[i++]));
	}
}

static void	list_paths(FILE *file, t_reverse_graph *graph, int state_id,
		char *path, int depth, unsigned long long *solution_number)
{
	t_brutestate	next;
	int			next_id;
	int			move_index;

	if (graph->distance[state_id] == 0)
	{
		fprintf(file, "    %llu. ", ++(*solution_number));
		write_path(file, path, depth);
		fprintf(file, "\n");
		return ;
	}
	move_index = 0;
	while (move_index < 11)
	{
		next = graph->states[state_id];
		brute_apply_move(&next, g_moves[move_index], graph->n);
		next_id = calculate_state_id(&next, graph->n);
		if (graph->distance[next_id] == graph->distance[state_id] - 1)
		{
			path[depth] = g_moves[move_index];
			list_paths(file, graph, next_id, path, depth + 1,
				solution_number);
		}
		move_index++;
	}
}

static int	next_permutation(int *values, int n)
{
	int	left;
	int	right;
	int	successor;
	int	temp;

	left = n - 2;
	while (left >= 0 && values[left] >= values[left + 1])
		left--;
	if (left < 0)
		return (0);
	successor = n - 1;
	while (values[successor] <= values[left])
		successor--;
	temp = values[left];
	values[left] = values[successor];
	values[successor] = temp;
	right = n - 1;
	left++;
	while (left < right)
	{
		temp = values[left];
		values[left++] = values[right];
		values[right--] = temp;
	}
	return (1);
}

static void	format_permutation(char *text, size_t size, const int *values,
		int n)
{
	size_t	used;
	int		written;
	int		i;

	used = 0;
	i = 0;
	text[0] = '\0';
	while (i < n && used < size)
	{
		written = snprintf(text + used, size - used, "%s%d",
			(i > 0) ? " " : "", values[i]);
		if (written < 0 || (size_t)written >= size - used)
			return ;
		used += (size_t)written;
		i++;
	}
}

static void	write_permutation_result(FILE *file, t_reverse_graph *graph,
		const int *values)
{
	t_brutestate		state;
	char				permutation[64];
	char				path[PATH_MAX_MOVES];
	unsigned long long	solution_number;
	unsigned long long	solutions;
	int				state_id;
	int				i;

	state.split = graph->n;
	i = 0;
	while (i < graph->n)
	{
		state.value[i] = values[i];
		i++;
	}
	state_id = calculate_state_id(&state, graph->n);
	solutions = count_paths(graph, state_id);
	format_permutation(permutation, sizeof(permutation), values, graph->n);
	fprintf(file, "%-15s | %5d | %10llu\n", permutation,
		graph->distance[state_id], solutions);
	solution_number = 0;
	list_paths(file, graph, state_id, path, 0, &solution_number);
}

static FILE	*open_report(char *filename, size_t size, int n,
		struct tm *time_info)
{
	snprintf(filename, size,
		"push_swap_bfs_all_paths_n%d_%04d-%02d-%02d_%02d-%02d-%02d.txt", n,
		time_info->tm_year + 1900, time_info->tm_mon + 1,
		time_info->tm_mday, time_info->tm_hour, time_info->tm_min,
		time_info->tm_sec);
	return (fopen(filename, "wx"));
}

static int	write_report(t_reverse_graph *graph)
{
	char		filename[256];
	int			values[7];
	time_t		now;
	struct tm	*time_info;
	FILE		*file;
	int			permutation_count;
	int			i;

	now = time(NULL);
	time_info = localtime(&now);
	if (time_info == NULL)
		return (ERROR);
	file = open_report(filename, sizeof(filename), graph->n, time_info);
	if (file == NULL)
		return (perror("Could not create a new BFS path report"), ERROR);
	permutation_count = factorial(graph->n);
	fprintf(file, "PUSH_SWAP REVERSE BFS - ALL SHORTEST PATHS\n");
	fprintf(file, "==========================================\n\n");
	fprintf(file, "n: %d\n", graph->n);
	fprintf(file, "initial permutations: %d\n", permutation_count);
	fprintf(file, "complete graph states: %d\n", graph->state_count);
	fprintf(file, "states discovered: %d\n", graph->discovered);
	fprintf(file, "maximum optimal distance: %d\n", graph->max_distance);
	fprintf(file, "generated: %04d-%02d-%02d %02d:%02d:%02d\n\n",
		time_info->tm_year + 1900, time_info->tm_mon + 1,
		time_info->tm_mday, time_info->tm_hour, time_info->tm_min,
		time_info->tm_sec);
	fprintf(file, "PERMUTATION     | MOVES | # SOLUTIONS\n");
	fprintf(file, "----------------+-------+------------\n");
	i = 0;
	while (i < graph->n)
	{
		values[i] = i;
		i++;
	}
	i = 0;
	while (i < permutation_count)
	{
		write_permutation_result(file, graph, values);
		fprintf(file, "\n");
		i++;
		if (i < permutation_count && !next_permutation(values, graph->n))
			return (fclose(file), ERROR);
	}
	if (graph->count_overflow)
		fprintf(file, "WARNING: at least one path count exceeded ULLONG_MAX.\n");
	if (fclose(file) != 0)
		return (ERROR);
	printf("Reverse BFS all-path report written to: %s\n", filename);
	return (SUCCESS);
}

int	main(int argc, char **argv)
{
	t_reverse_graph	graph;
	int				n;
	int				status;

	if (argc != 2)
		return (fprintf(stderr, "Usage: %s <n from 2 to 7>\n", argv[0]), 1);
	n = atoi(argv[1]);
	if (n < 2 || n > 7)
		return (fprintf(stderr, "n must be from 2 to 7\n"), 1);
	memset(&graph, 0, sizeof(graph));
	if (allocate_graph(&graph, n) == ERROR)
		return (free_graph(&graph), perror("Could not allocate graph"), 1);
	if (build_reverse_graph(&graph) == ERROR)
		return (free_graph(&graph),
			fprintf(stderr, "Reverse BFS did not discover the full graph\n"), 1);
	status = write_report(&graph);
	free_graph(&graph);
	return (status == ERROR);
}
