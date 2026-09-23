/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:22:29 by hnah              #+#    #+#             */
/*   Updated: 2026/09/23 18:02:22 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include "../libft/ryker_libft.h"
# include <stdio.h>
# include <unistd.h>

# define BUBBLE_SORT_MAX_500	249500
# define BRUTE_MAX_N			10
# define BRUTE_TOTAL_N_PLUS_1_FACTORIAL	39916800


typedef struct s_circle_buf
{
	int	buf[501];
	int	capacity;
	int	read_idx;
	int	write_idx;
}	circle_buf;

typedef struct s_soln
{
	char	**ans;
	int		*ans_len;
	int		cur;
	int		step;
}	soln;

// FYI: Don't (int) type apparently - (unsigned char) is 0..255; values are normalized ranks and split only needs 0..n. 
// So is safe for any brute-force size CPU can go bzzzzzz!~ \(^o^)/ 
typedef struct s_brutestate
{
	unsigned char	value[BRUTE_MAX_N];
	unsigned char	split;
}	t_brutestate;

// FYI: Apparently reordering t_brutenode members can minimize alignment padding:
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
# define ERR_PARSE_INPUT		3
# define ERR_SORT_INPUT		4

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

/* debugger */
void	debug_print_int_array(const int *array, int size);
void	cbuf_print(circle_buf *stack, char name);
void	cbuf_print_stacks(circle_buf *a, circle_buf *b);
void	debug_print_soln(const soln *x);
int		print_first_soln(const soln *x);

/* do not submit*/
int	debug_hidden_bfs(soln *real, circle_buf *a, circle_buf *b);
int	extract_chunk_optimal(soln *x, circle_buf *a, circle_buf *b, int min, int max);

/* parser */
int	count_int_in_str(char *str, int *count, int *values);
int	rank_values(const int count, const int *values, int *ranks);

/* solutions */
void	append(soln *x, char move);
int	soln_init(soln *x, const int soln_num, const int steps_limit);
int	get_order_top_three(circle_buf *a);
int	solve(soln *x, circle_buf *a, circle_buf *b, int count);

/* bfs solver */
int	brute_solve(soln *x, circle_buf *a, circle_buf *b);
int	brute_state_exists(t_brutestate *temp, t_brutenode *nodes,
		int total, int n);
int	is_brute_goal(t_brutestate *state, int n);
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

/* bfs helpers */
void	brute_swap_at(t_brutestate *state, int a, int b);
void	brute_rotate_left(t_brutestate *state, int start, int end);
void	brute_rotate_right(t_brutestate *state, int start, int end);
int		calculate_state_id(t_brutestate *a, int n);


/* circle_buf core */
void	cbuf_init_ab(circle_buf *a, circle_buf *b, int count);
int	cbuf_read_at(circle_buf *stack, int index, int *value);
int	cbuf_is_empty(circle_buf *stack);
int	cbuf_is_full(circle_buf *stack);
int	cbuf_len(circle_buf *stack);
// int	cbuf_free(circle_buf *stack);

/* circle_buf operations */
int	cbuf_push_top(circle_buf *stack, int number);
int	cbuf_push_bottom(circle_buf *stack, int number);
int	cbuf_pop_bottom(circle_buf *stack, int *pop_number);
int	cbuf_pop_top(circle_buf *stack, int *pop_number);
int	cbuf_swap_top(circle_buf *stack);
int	cbuf_rotate(circle_buf *stack);
int	cbuf_rev_rotate(circle_buf *stack);

/* push_swap operations */
int	sa(soln *x, circle_buf *a);
int	sb(soln *x, circle_buf *b);
int	ss(soln *x, circle_buf *a, circle_buf *b);
int	pa(soln *x, circle_buf *a, circle_buf *b);
int	pb(soln *x, circle_buf *a, circle_buf *b);
int	ra(soln *x, circle_buf *a);
int	rb(soln *x, circle_buf *b);
int	rr(soln *x, circle_buf *a, circle_buf *b);
int	rra(soln *x, circle_buf *a);
int	rrb(soln *x, circle_buf *b);
int	rrr(soln *x, circle_buf *a, circle_buf *b);

#endif
