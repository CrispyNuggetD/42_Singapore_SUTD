/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 13:22:29 by hnah              #+#    #+#             */
/*   Updated: 2026/04/20 17:03:57 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

#include <stdarg.h>
#include "libft/libft.h"
#include <limits.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>

typedef struct s_cbuf
{
	int	*buffer;
	int	capacity;
	int	read_idx;
	int	write_idx;
}	cbuf;

typedef struct s_input
{
	int	**sorted_input;
	int input_len;
}	input;


# define FLAG_MINUS 1
# define FLAG_ZERO  2
# define FLAG_PREC  4
# define FLAG_HASH  8
# define FLAG_SPACE 16
# define FLAG_PLUS  32

/* typedef enum e_length
{
	LEN_NONE,
	LEN_HH,
	LEN_H,
	LEN_L,
	LEN_LL
}	t_length; */

// typedef int	(*t_handler)(t_context *context);

/* cbuf core */
int	cbuf_init(cbuf *stack, int size);
int	cbuf_is_empty(cbuf *stack);
int	cbuf_is_full(cbuf *stack);
int	cbuf_len(cbuf *stack);
void	cbuf_free(cbuf *stack);

/* cbuf operations */
int	cbuf_push_top(cbuf *stack, int number);
int	cbuf_push_bottom(cbuf *stack, int number);
int	cbuf_pop_top(cbuf *stack, int *read_number);
int	cbuf_swap_top(cbuf *stack);
int	cbuf_rotate(cbuf *stack);
int	cbuf_rev_rotate(cbuf *stack);

/* push_swap operations */
int	sa(cbuf *a);
int	sb(cbuf *b);
int	ss(cbuf *a, cbuf *b);
int	pa(cbuf *a, cbuf *b);
int	pb(cbuf *a, cbuf *b);
int	ra(cbuf *a);
int	rb(cbuf *b);
int	rr(cbuf *a, cbuf *b);
int	rra(cbuf *a);
int	rrb(cbuf *b);
int	rrr(cbuf *a, cbuf *b);

#endif
#define SUCCESS				0
#define ERROR				1
#define ERR_INVALID_INPUT	2
#define ERR_PARSE_INPUT		3
#define ERR_SORT_INPUT		4