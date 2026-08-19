/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:22:29 by hnah              #+#    #+#             */
/*   Updated: 2026/08/19 20:37:11 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include "libft/0_libft/libft.h"
# include <stdio.h>
# include <unistd.h>

typedef struct s_cbuf
{
	int	buf[501];
	int	capacity;
	int	read_idx;
	int	write_idx;
}	cbuf;

typedef struct s_soln
{
	char	**ans;
	int		*ans_len;
	int		cur;
	int		step;
}	soln;

typedef struct s_brutestate
{
	int	value[7];
	int	split;
}	t_brutestate;

typedef struct s_brutenode
{
	t_brutestate	state;
	int				parent;
	int				move;
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

# define BUBBLE_SORT_MAX_500	249500
# define BRUTE_MAX_N			8
# define BRUTE_TOTAL_N_PLUS_1_FACTORIAL	362880

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

/* typedef enum e_length
{
	LEN_NONE,
	LEN_HH,
	LEN_H,
	LEN_L,
	LEN_LL
}	t_length; */

// typedef int	(*t_handler)(t_context *context);

/* debugger */
void	debug_print_int_array(const int *array, int size);
void	cbuf_print(cbuf *stack, char name);
void	cbuf_print_stacks(cbuf *a, cbuf *b);
void	debug_print_soln(const soln *x);
int		print_first_soln(const soln *x);

/* parser */
int	count_int_in_str(char *str, int *count, int *values);
int	rank_values(const int count, const int *values, int *ranks);

/* solutions */
void	append(soln *x, char move);
int	soln_init(soln *x, const int soln_num, const int steps_limit);
int	get_order_top_three(cbuf *a);
int	solve(soln *x, cbuf *a, cbuf *b, int count);

/* bfs solver */
int	brute_solve(soln *x, cbuf *a, cbuf *b);
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


/* cbuf core */
void	cbuf_info(cbuf *a, cbuf *b, int count);
int	cbuf_is_empty(cbuf *stack);
int	cbuf_is_full(cbuf *stack);
int	cbuf_len(cbuf *stack);
// int	cbuf_free(cbuf *stack);

/* cbuf operations */
int	cbuf_push_top(cbuf *stack, int number);
int	cbuf_push_bottom(cbuf *stack, int number);
int	cbuf_pop_bottom(cbuf *stack, int *pop_number);
int	cbuf_pop_top(cbuf *stack, int *pop_number);
int	cbuf_swap_top(cbuf *stack);
int	cbuf_rotate(cbuf *stack);
int	cbuf_rev_rotate(cbuf *stack);

/* push_swap operations */
int	sa(soln *x, cbuf *a);
int	sb(soln *x, cbuf *b);
int	ss(soln *x, cbuf *a, cbuf *b);
int	pa(soln *x, cbuf *a, cbuf *b);
int	pb(soln *x, cbuf *a, cbuf *b);
int	ra(soln *x, cbuf *a);
int	rb(soln *x, cbuf *b);
int	rr(soln *x, cbuf *a, cbuf *b);
int	rra(soln *x, cbuf *a);
int	rrb(soln *x, cbuf *b);
int	rrr(soln *x, cbuf *a, cbuf *b);

#endif
