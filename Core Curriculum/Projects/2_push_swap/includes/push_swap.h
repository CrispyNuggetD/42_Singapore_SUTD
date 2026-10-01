/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:22:29 by hnah              #+#    #+#             */
/*   Updated: 2026/10/01 19:11:02 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include "../libft/ryker_libft.h"
# include <stdio.h>
# include <unistd.h>
# include "algorithm.h"

# define MAX_MOVES_CONSIDERED				10000
# define BRUTE_MAX_N						10
# define BRUTE_TOTAL_N_PLUS_1_FACTORIAL		39916800
# define SKIP_OTHER_ALGO_AFTER_BFS			0

/* Search depth counts insertions and must be at least one. */
# define LOOKAHEAD_DEPTH_100					12
# define LOOKAHEAD_DEPTH_500					8
# define LOOKAHEAD_DEPTH_FIRST_MOVE				12

/* Execution limits count insertions, must be >= 1, and cap at path length. */
# define EXECUTE_LIMIT_100						10
# define EXECUTE_LIMIT_500						6
# define EXECUTE_LIMIT_FIRST_MOVE				10

/* Capacity must cover every configured lookahead depth. */
# define GREEDY_PATH_CAPACITY 14

/* Zero disables diagnostics; positive levels enable debug output. */
# define DEBUG 								1

typedef struct s_circle_buf
{
	int	buf[501];
	int	capacity;
	int	read_idx;
	int	write_idx;
}	t_circle_buf;

typedef struct s_soln
{
	char	**ans;
	int		*ans_len;
	int		cur;
	int		step;
}	t_soln;

/* Normalised ranks; split is the number of values in A. */
typedef struct s_brutestate
{
	unsigned char	value[BRUTE_MAX_N];
	unsigned char	split;
}	t_brutestate;

typedef struct s_brutenode
{
	int				parent;
	t_brutestate	state;
	char			move;
}	t_brutenode;

# define FLAG_MINUS 1
# define FLAG_ZERO  2
# define FLAG_PREC  4
# define FLAG_HASH  8
# define FLAG_SPACE 16
# define FLAG_PLUS  32

# define SUCCESS				0
# define SKIP				0
# define ERROR				1
# define ERR_INVALID_INPUT	2
# define ERR_INVALID_PARAM	2
# define ERR_PARSE_INPUT		3
# define ERR_SORT_INPUT		4

# define A		0
# define B		1

# define SA		'1'
# define SB		'2'
# define SS		'3'
# define PA		'4'
# define PB		'5'
# define RA		'6'
# define RB		'7'
# define RR		'8'
# define RRA	'9'
# define RRB	'A'
# define RRR	'B'

/* Debug output; emission depends on DEBUG. */
void	debug_print_bfs_memory(size_t node_count);
void	debug_print_int_array(const int *array, int size);
void	cbuf_print(t_circle_buf *stack, char name);
void	cbuf_print_stacks(t_circle_buf *a, t_circle_buf *b);
void	debug_print_soln(const t_soln *x, t_circle_buf *a_ori);
void	debug_lis_length(int length);
void	debug_bfs_progress(int expanded, int discovered, int capacity);
void	debug_bfs_start(int n, int capacity);
void	debug_bfs_end(int moves);
void	debug_bfs_alloc(size_t bytes);
void	debug_print_message(const char *message);
void	debug_bfs_run(int run, int total, int start, int end);
void	debug_chunk_result(int start, int end, int extraction, int bfs);
void	debug_chunk_route(int start, int end, int cost, const char *direction);
void	debug_chunk_turn(int turn);
void	debug_total_moves(int total);

/* Solution output: stdout, independent of DEBUG. */
int		print_best_soln(const t_soln *x);

/* parser */
int		count_int_in_str(char *str, int *count, int *values);
int		rank_values(const int count, const int *values, int *ranks);

/* Solution storage and shared sorting helpers. */
/* Caller ensures answer capacity. NULL x skips recording. */
void	append_move_to_soln(t_soln *x, char move);
int		soln_init(t_soln *x, const int soln_num, const int steps_limit);
/* Start an allocated answer slot and copy the original stacks. */
int		new_soln_init(t_soln *x, t_circle_buf stacks[2], t_circle_buf *a_ori,
			t_circle_buf *b_ori);
int		get_order_top_three(t_circle_buf *a);
int		hardcode_three(t_soln *x, t_circle_buf *a);
int		rot_a_min_rotation(t_circle_buf *a, int *rotations);
int		rot_a_min_to_top(t_soln *x, t_circle_buf *a);

/* Solver entry points. */
int		solve(t_soln *x, t_circle_buf *a, t_circle_buf *b, int count);
int		greedy_reinsertion(t_soln *x, t_circle_buf stacks[2],
			t_algorithm algo);
int		brute_solve(t_soln *x, t_circle_buf *a, t_circle_buf *b, int count);
/* Record a 1..4-value answer; requires a fresh slot, ranked A and empty B. */
int		get_precomputed_bfs(t_soln *x, t_circle_buf *a, int count);

/* bfs solver */
void	gen_brute_state(t_brutestate *state, t_circle_buf *a, t_circle_buf *b);
int		brute_state_exists(t_brutestate *temp, t_brutenode *nodes,
			int total, int n);
int		is_brute_goal(t_brutestate *state, int n);
void	brute_apply_move(t_brutestate *state, char move, int n);

/* bfs operations */
void	brute_sa(t_brutestate *state);
void	brute_sb(t_brutestate *state, int n);
void	brute_ss(t_brutestate *state, int n);
void	brute_pa(t_brutestate *state, int n);
void	brute_pb(t_brutestate *state);
void	brute_ra(t_brutestate *state);
void	brute_rb(t_brutestate *state, int n);
void	brute_rr(t_brutestate *state, int n);
void	brute_rra(t_brutestate *state);
void	brute_rrb(t_brutestate *state, int n);
void	brute_rrr(t_brutestate *state, int n);

/* BFS state counts and indexing. */
/* Return -1 outside factorial range 0..11 or BFS input size 0..10. */
int		factorial_max_11(int n);
int		bfs_possible_states(int n);
int		state_was_visited(const unsigned char *visited, int state_id);
void	mark_state_visited(unsigned char *visited, int state_id);
int		calculate_state_id(t_brutestate *a, int n);

/* BFS array helpers: physical indices and inclusive ranges. */
void	brute_swap_at(t_brutestate *state, int a, int b);
void	brute_rotate_left(t_brutestate *state, int start, int end);
void	brute_rotate_right(t_brutestate *state, int start, int end);

/* Circular-buffer access. */
void	cbuf_init_ab(t_circle_buf *a, t_circle_buf *b, int count);
int		cbuf_read_at(t_circle_buf *stack, int index, int *value);
int		cbuf_is_empty(t_circle_buf *stack);
int		cbuf_is_full(t_circle_buf *stack);
int		cbuf_len(t_circle_buf *stack);
int		cbuf_opp_moves(t_circle_buf *stack, int moves);
int		get_cbuf_lis(t_circle_buf *stack, char keep_flags[500]);

/* Circular-buffer mutations. */
int		cbuf_push_top(t_circle_buf *stack, int number);
int		cbuf_push_bottom(t_circle_buf *stack, int number);
int		cbuf_pop_bottom(t_circle_buf *stack, int *pop_number);
int		cbuf_pop_top(t_circle_buf *stack, int *pop_number);
int		cbuf_swap_top(t_circle_buf *stack);
int		cbuf_rotate(t_circle_buf *stack);
int		cbuf_rev_rotate(t_circle_buf *stack);

/* push_swap operations */
int		sa(t_soln *x, t_circle_buf *a);
int		sb(t_soln *x, t_circle_buf *b);
int		ss(t_soln *x, t_circle_buf *a, t_circle_buf *b);
int		pa(t_soln *x, t_circle_buf *a, t_circle_buf *b);
int		pb(t_soln *x, t_circle_buf *a, t_circle_buf *b);
int		ra(t_soln *x, t_circle_buf *a);
int		rb(t_soln *x, t_circle_buf *b);
int		rr(t_soln *x, t_circle_buf *a, t_circle_buf *b);
int		rra(t_soln *x, t_circle_buf *a);
int		rrb(t_soln *x, t_circle_buf *b);
int		rrr(t_soln *x, t_circle_buf *a, t_circle_buf *b);

#endif
