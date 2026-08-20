/* ************************************************************************** */
/*                                                                            */
/*             DO NOT SUBMIT - BFS ALL-PERMUTATIONS ANALYSER                 */
/*                                                                            */
/* ************************************************************************** */

/*
 * Generates every permutation for n, solves each one with brute_solve(),
 * and writes one combined report.  This file uses unauthorised functions and
 * must stay outside the submitted push_swap source list.
 */

#include "../push_swap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEBUG_SOLUTION_MAX 128

typedef struct s_analysis_result
{
	int	values[7];
	int	n;
	int	moves;
	char	solution[DEBUG_SOLUTION_MAX];
} t_analysis_result;

static int	factorial(int n)
{
	int	result;

	result = 1;
	while (n > 1)
		result *= n--;
	return (result);
}

static void	swap_int(int *a, int *b)
{
	int	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

static int	next_permutation(int *values, int n)
{
	int	left;
	int	right;
	int	successor;

	left = n - 2;
	while (left >= 0 && values[left] >= values[left + 1])
		left--;
	if (left < 0)
		return (0);
	successor = n - 1;
	while (values[successor] <= values[left])
		successor--;
	swap_int(&values[left], &values[successor]);
	right = n - 1;
	left++;
	while (left < right)
		swap_int(&values[left++], &values[right--]);
	return (1);
}

static int	solve_permutation(t_analysis_result *result,
		const int *values, int n)
{
	cbuf	a;
	cbuf	b;
	soln	x;
	char	*answers[1];
	int	answer_len;
	int	i;

	i = 0;
	while (i < n)
	{
		a.buf[i] = values[i];
		result->values[i] = values[i];
		i++;
	}
	cbuf_info(&a, &b, n);
	answers[0] = result->solution;
	answer_len = 0;
	x.ans = answers;
	x.ans_len = &answer_len;
	x.cur = 0;
	x.step = 0;
	if (brute_solve(&x, &a, &b) == ERROR
		|| answer_len >= DEBUG_SOLUTION_MAX)
		return (ERROR);
	result->n = n;
	result->moves = answer_len;
	result->solution[answer_len] = '\0';
	return (SUCCESS);
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

static void	write_solution(FILE *file, const t_analysis_result *result)
{
	int	i;

	i = 0;
	while (i < result->moves)
	{
		if (i > 0)
			fprintf(file, " ");
		fprintf(file, "%s", move_name(result->solution[i]));
		i++;
	}
}

static void	format_permutation(char *text, size_t size,
		const t_analysis_result *result)
{
	size_t	used;
	int		written;
	int		i;

	used = 0;
	i = 0;
	text[0] = '\0';
	while (i < result->n && used < size)
	{
		written = snprintf(text + used, size - used,
			"%s%d", (i > 0) ? " " : "", result->values[i]);
		if (written < 0 || (size_t)written >= size - used)
			return ;
		used += (size_t)written;
		i++;
	}
}

static void	write_result(FILE *file, const t_analysis_result *result)
{
	char	permutation[64];

	format_permutation(permutation, sizeof(permutation), result);
	fprintf(file, "%-15s | %5d | ", permutation, result->moves);
	write_solution(file, result);
	fprintf(file, "\n");
}

static int	compare_permutation(const void *a, const void *b)
{
	const t_analysis_result	*left;
	const t_analysis_result	*right;
	int					i;

	left = (const t_analysis_result *)a;
	right = (const t_analysis_result *)b;
	i = 0;
	while (i < left->n)
	{
		if (left->values[i] != right->values[i])
			return ((left->values[i] > right->values[i]) * 2 - 1);
		i++;
	}
	return (0);
}

static int	compare_distance_descending(const void *a, const void *b)
{
	const t_analysis_result	*left;
	const t_analysis_result	*right;

	left = (const t_analysis_result *)a;
	right = (const t_analysis_result *)b;
	if (left->moves != right->moves)
		return ((left->moves < right->moves) * 2 - 1);
	return (compare_permutation(a, b));
}

static void	write_section(FILE *file, const char *title,
		const t_analysis_result *results, int count)
{
	int	i;

	fprintf(file, "SORT: %s\n", title);
	fprintf(file, "---------------------------------\n\n");
	fprintf(file, "PERMUTATION     | MOVES | SOLUTION\n");
	fprintf(file, "----------------+-------+--------------------\n");
	i = 0;
	while (i < count)
		write_result(file, &results[i++]);
}

static FILE	*open_report(char *filename, size_t size, int n,
		struct tm *time_info)
{
	snprintf(filename, size,
		"push_swap_bfs_all_n%d_%04d-%02d-%02d_%02d-%02d-%02d.txt", n,
		time_info->tm_year + 1900, time_info->tm_mon + 1,
		time_info->tm_mday, time_info->tm_hour, time_info->tm_min,
		time_info->tm_sec);
	return (fopen(filename, "wx"));
}

static int	write_report(t_analysis_result *results, int count, int n)
{
	char		filename[256];
	time_t		now;
	struct tm	*time_info;
	FILE		*file;

	now = time(NULL);
	time_info = localtime(&now);
	if (time_info == NULL)
		return (ERROR);
	file = open_report(filename, sizeof(filename), n, time_info);
	if (file == NULL)
	{
		perror("Could not create a new BFS analysis file");
		return (ERROR);
	}
	fprintf(file, "PUSH_SWAP BFS ANALYSIS\n======================\n\n");
	fprintf(file, "n: %d\npermutations: %d\n", n, count);
	fprintf(file, "generated: %04d-%02d-%02d %02d:%02d:%02d\n\n",
		time_info->tm_year + 1900, time_info->tm_mon + 1,
		time_info->tm_mday, time_info->tm_hour, time_info->tm_min,
		time_info->tm_sec);
	qsort(results, count, sizeof(*results), compare_permutation);
	write_section(file, "PERMUTATION ASCENDING", results, count);
	fprintf(file, "\n\n");
	qsort(results, count, sizeof(*results), compare_distance_descending);
	write_section(file, "OPTIMAL DISTANCE DESCENDING", results, count);
	if (fclose(file) != 0)
		return (ERROR);
	printf("BFS all-permutations analysis written to: %s\n", filename);
	return (SUCCESS);
}

int	main(int argc, char **argv)
{
	t_analysis_result	*results;
	int				values[7];
	int				count;
	int				n;
	int				i;

	if (argc != 2)
		return (fprintf(stderr, "Usage: %s <n from 2 to 7>\n", argv[0]), 1);
	n = atoi(argv[1]);
	if (n < 2 || n > 7)
		return (fprintf(stderr, "n must be from 2 to 7\n"), 1);
	count = factorial(n);
	results = malloc(sizeof(*results) * (size_t)count);
	if (results == NULL)
		return (perror("Could not allocate analysis results"), 1);
	i = 0;
	while (i < n)
	{
		values[i] = i;
		i++;
	}
	i = 0;
	while (i < count)
	{
		if (solve_permutation(&results[i], values, n) == ERROR)
			return (free(results), fprintf(stderr, "BFS failed\n"), 1);
		i++;
		if (i < count && !next_permutation(values, n))
			return (free(results), fprintf(stderr, "Generator failed\n"), 1);
	}
	i = write_report(results, count, n);
	free(results);
	return (i == ERROR);
}
