/* ************************************************************************** */
/*                                                                            */
/*                  DO NOT SUBMIT - DEBUG / EXPERIMENTAL                      */
/*                                                                            */
/* ************************************************************************** */

/*
 * This file intentionally uses functions that are NOT authorised for the
 * submitted push_swap project.
 *
 * Keep this file OUT of the normal Makefile.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "DO_NOT_SUBMIT_DEBUG_bfs_results.h"

typedef struct s_debug_bfs_result
{
	int		*values;
	int		n;
	int		moves;
	char	*solution;
} t_debug_bfs_result;

static void	make_filename(char *filename, size_t size,
		const char *program_name)
{
	time_t		now;
	struct tm	*time_info;

	now = time(NULL);
	time_info = localtime(&now);
	if (time_info == NULL)
	{
		snprintf(filename, size, "%s_results.txt", program_name);
		return ;
	}
	snprintf(filename, size, "%s_%04d-%02d-%02d_%02d-%02d-%02d.txt",
		program_name, time_info->tm_year + 1900, time_info->tm_mon + 1,
		time_info->tm_mday, time_info->tm_hour, time_info->tm_min,
		time_info->tm_sec);
}

static void	write_title(FILE *file, int n, int count)
{
	fprintf(file, "PUSH_SWAP BFS ANALYSIS\n");
	fprintf(file, "======================\n\n");
	fprintf(file, "n: %d\n", n);
	fprintf(file, "permutations: %d\n\n", count);
}

static void	write_table_header(FILE *file, const char *sort_name)
{
	fprintf(file, "SORT: %s\n", sort_name);
	fprintf(file, "---------------------------------------------\n\n");
	fprintf(file, "PERMUTATION | MOVES | SOLUTION\n");
	fprintf(file, "------------+-------+-------------------------\n");
}

static void	write_permutation(FILE *file,
		const t_debug_bfs_result *result)
{
	int	i;

	i = 0;
	while (i < result->n)
	{
		fprintf(file, "%d", result->values[i]);
		if (i + 1 < result->n)
			fprintf(file, " ");
		i++;
	}
}

static void	write_result(FILE *file, const t_debug_bfs_result *result)
{
	write_permutation(file, result);
	fprintf(file, " | %5d | ", result->moves);
	if (result->solution != NULL)
		fprintf(file, "%s", result->solution);
	fprintf(file, "\n");
}

static void	write_result_array(FILE *file,
		const t_debug_bfs_result *results, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		write_result(file, &results[i]);
		i++;
	}
}

static int	compare_moves_ascending(const void *a, const void *b)
{
	const t_debug_bfs_result	*left;
	const t_debug_bfs_result	*right;

	left = (const t_debug_bfs_result *)a;
	right = (const t_debug_bfs_result *)b;
	if (left->moves < right->moves)
		return (-1);
	return (left->moves > right->moves);
}

static int	compare_moves_descending(const void *a, const void *b)
{
	return (compare_moves_ascending(b, a));
}

void	debug_write_bfs_results(t_debug_bfs_result *results, int count, int n)
{
	char	filename[256];
	FILE	*file;

	if (results == NULL || count <= 0 || n <= 0)
	{
		fprintf(stderr, "Cannot write BFS results: invalid arguments\n");
		return ;
	}
	make_filename(filename, sizeof(filename), "push_swap_bfs");
	file = fopen(filename, "w");
	if (file == NULL)
	{
		perror("Could not create BFS result file");
		return ;
	}
	write_title(file, n, count);
	write_table_header(file, "BFS DISTANCE ASCENDING");
	qsort(results, count, sizeof(t_debug_bfs_result),
		compare_moves_ascending);
	write_result_array(file, results, count);
	fprintf(file, "\n\n");
	write_table_header(file, "BFS DISTANCE DESCENDING");
	qsort(results, count, sizeof(t_debug_bfs_result),
		compare_moves_descending);
	write_result_array(file, results, count);
	if (fclose(file) != 0)
	{
		perror("Could not finish writing BFS result file");
		return ;
	}
	printf("BFS analysis written to: %s\n", filename);
}

void	debug_log_bfs_run(const int *values, int n,
		const char *solution, int solution_len)
{
	t_debug_bfs_result	result;
	char				*terminated_solution;

	if (values == NULL || solution == NULL || n <= 0 || solution_len < 0)
	{
		fprintf(stderr, "Cannot log BFS run: invalid arguments\n");
		return ;
	}
	terminated_solution = malloc((size_t)solution_len + 1);
	if (terminated_solution == NULL)
	{
		perror("Could not allocate BFS debug solution");
		return ;
	}
	if (solution_len > 0)
		memcpy(terminated_solution, solution, (size_t)solution_len);
	terminated_solution[solution_len] = '\0';
	result.values = (int *)values;
	result.n = n;
	result.moves = solution_len;
	result.solution = terminated_solution;
	debug_write_bfs_results(&result, 1, n);
	free(terminated_solution);
}
