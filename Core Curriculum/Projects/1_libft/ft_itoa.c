/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 11:37:33 by hnah              #+#    #+#             */
/*   Updated: 2025/12/07 17:17:24 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	int_len(int n);

// len-- in backwards filling loop says number length decrements every loop
char	*ft_itoa(int n)
{
	char	*str;
	int		len;
	int		is_negative;
	long	n_long;

	is_negative = (n < 0);
	len = int_len(n);
	n_long = (long)n;
	if (is_negative)
		n_long = -n_long;
	str = malloc(sizeof(char) * (is_negative + len + 1));
	if (!str)
		return (NULL);
	str[is_negative + len] = '\0';
	if (is_negative)
		str[0] = '-';
	if (n_long == 0)
		str[0] = '0';
	while (len > 0)
	{
		str[is_negative + len - 1] = '0' + (n_long % 10);
		n_long /= 10;
		len--;
	}
	return (str);
}
// Recap:
// len-- in backwards filling loop says number length decrements every loop

static int	int_len(int n)
{
	int	len;

	len = 0;
	if (n == 0)
	{
		len = 1;
		return (len);
	}
	while (n != 0)
	{
		n /= 10;
		len++;
	}
	return (len);
}

/*
int	main(void)
{
	char	*output;

	output = ft_itoa(-2147483648);
	if (!output)
		return (1);
	write(1, output, ft_strlen(output));
	output = ft_itoa(+2147483647);
	if (!output)
		return (1);
	write(1, output, ft_strlen(output));
	output = ft_itoa(0);
	if (!output)
		return (1);
	write(1, output, ft_strlen(output));
	output = ft_itoa(-1);
	if (!output)
		return (1);
	write(1, output, ft_strlen(output));
	output = ft_itoa(-2147);
	if (!output)
		return (1);
	write(1, output, ft_strlen(output));
	free(output);
	return (0);
}
*/

/* ************************************************************************** */
/* ************************************************************************** */
/* ************************************************************************** */

/* 
 * DISCUSSION:
 *
 * While this is the canonical way, there's also another all-negative way
 * that settles the INT_MIN overflow issue (below). It uses the idea that
 * the % remainder operator (not "true" maths "modulo") keeps the sign from 
 * the dividend.
 * 
 * Goal for ft_itoa:
 * Convert an int n to a freshly malloc’d C string.
 *
 * Key constraints:
 * Works for all int values (including INT_MIN)
 * Allocates exact length (no waste)
 * No hardcoded magic constants like 12 if possible
 * Caller must be able to free the result
 */

/* ************************************************************************** */
/* ************************************************************************** */
/* ************************************************************************** */

/* ************************************************************************** */

/* 
 * (ADDITIONAL INFO NOT REQUIRED FOR EVAL):
 * Alternative approach B:  All-negative route (no long)
 *
 * ChatGPT thought this was "fun" or "elegant", but if I'm not wrong real
 * use case is only when using 32-bit operations only embedded systems
 * with no long support OR portable code since C standards only guarantees 
 * 32-bit longs.
 *
 * A PRIMER:
 * In C, strictly speaking, the '%' "modulo operator" is a "remainder"
 * operator, defined in C standards as "(a / b) * b + (a % b) == a".
 *
 * This is not the same as TRUE modular arithmetic, where the result is
 * ALWAYS in [0, b-1].
 *
 * ILLUSTRATION OF "REMAINDER OPERATOR":
 *   5 %  3 =  2
 *   5 % -3 =  2
 *  -5 % -3 = -2
 *  -5 %  3 = -2  <- BUT IMPORTANT! NOTE SIGN IS SAME AS DIVIDEND. Ans != 1
 * 
 * This is why the negative route works (remainder operator keeps sign)
 */

/* ************************************************************************** */

/* 
 * Idea:
 * Keep n as negative, and when you extract digits, do:
 *
 * last_digit = -(n % 10) (gives positive 0–9)
 * then n /= 10 (still negative, approaching 0)
 *
 * Algorithm sketch:
 * (Similar to canonical) If n == 0, handle "0" special case.
 * (Similar too) is_negative = (n < 0)
 *
 * DIFFERENCE: Count digits while n != 0 using division by 10,
 * BUT keep it negative.
 *
 * Allocate length = digits + (is_negative ? 1 : 0) + 1 for \0.
 *
 * Fill from end:
 *
 * (Using the negative route method)
 * digit = -(n % 10)
 * str[i] = '0' + digit
 * n /= 10
 *
 * If negative, str[0] = '-'.
 */

/* ************************************************************************** */

/* PROS OF METHOD:
 * NO need for a long
 * Works for INT_MIN naturally
 * Mathematically neat
 *
 * CONS AS A RESULT:
 * Slightly unintuitive: % on negative numbers is confusing
 * (LOL) Can be harder to reason about under sleep deprivation
 * w/o internalising negative modulo semantics in C, it’s bug-prone
 *
 * TL;DR ChatGPT: Still perfectly valid, and elegant once brain locks it in.
 */

/* EOF */
