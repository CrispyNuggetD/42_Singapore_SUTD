/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 11:37:33 by hnah              #+#    #+#             */
/*   Updated: 2025/12/05 09:57:42 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_itoa(int n)
{
	char	*str;
	int		is_negative;
	int		len;
	long	n_long;
	long	temp;

	n_long = (long)n;
	is_negative = (n < 0);
	if (n_long < 0)
		n_long = -n_long;
	len = 1;
	temp = n_long;
	while (temp / 10 != 0)
	{
		temp /= 10;
		len++;
	}
	str = malloc(sizeof(char) * (is_negative + len + 1));
	if (!str)
		return (NULL);
	str[len + is_negative] = '\0';
	if (n_long == 0)
		str[0] = '0';
	while (n_long > 0)
	{
		str[--len + is negative] = '0' + (n_long % 10); 
		/* DOESN'T WORK REWRITE ITOA WITH HELPER LEN COUNTER OR LIBC */
		n_long /= 10;
	}
	if (is_negative)
		str[--len] = '-';
	return (str);
}

int	main(void)
{
	int	test;

	test = 3;
	test += 1;
}

/* ************************************************************************** */
/* ************************************************************************** */
/* ************************************************************************** */

/* 
 * DISCUSSION:
 *
 * While there's the canonical way (more below), I chose my final code which
 * was an alternative approach based on how well the compiler "could've" 
 * optimised it if cc were given -O2 just like "in real-life" code, though I
 * found out 42 doesn't.(Obviously - It only compiles cc using the warning
 * flags).
 *
 * The final code is linked to my liking of efficient, super fast code,
 * suitable for ultra-low latency applications, though I might not even use
 * C language at that point...
 * 
 * Goal:
 * Convert an int n to a freshly malloc’d C string.
 *
 * Key constraints:
 * Works for all int values (including INT_MIN)
 * Allocates exact length (no waste)
 * No hardcoded magic constants like 12 if possible
 * Caller must be able to free the result
 *
 * ChatGPT provided 4 alternative approaches upon prompting as I wanted to
 * learn more approaches. Pros and Cons discussed below, such as space-time
 * tradeoff of my method.
 */

/* ************************************************************************** */
/* ************************************************************************** */
/* ************************************************************************** */

/*
 * Approach A (1 of 5): Canonical ft_itoa algorithm
 * (ChatGPT's vibecoding recommendation for 42)
 * 
 * --> Step 0 — Promote to wider type
 * 
 * Problem: INT_MIN cannot be negated in int range.
 * - Take input int n
 * - Create long nb = n;
 * (now nb can hold +2147483648 safely on usual 64-bit machines)
 * 
 * From here on, all logic is done using nb.
 * 
 * --> Step 1 — Determine if negative
 * is_negative = (nb < 0)
 * 
 * If negative:
 * (!) remember this (need one extra char for -)
 * 
 * set nb = -nb (safe now, because nb is long)
 * Now nb is non-negative, even if original was INT_MIN.
 * 
 * --> Step 2 — Count digits
 * 
 * We now have a non-negative nb.
 * 
 * If nb == 0, then digit count is 1 (special case)
 * Else:
 * count = 0
 * while nb > 0:
 * nb /= 10
 * count++
 * 
 * After this, count = number of decimal digits.
 * 
 * Then total length:
 * len = count
 * If is_negative, len++ for '-'
 * 
 * Plus one more for '\0' when allocating, so we will 
 * malloc(len + 1) bytes:
 * 
 * len characters
 * 1 null terminator
 * 
 * (Implementation detail: some people fold “+1 for \0” into 
 * len itself; doesn’t matter conceptually.)
 * 
 * --> Step 3 — Fill the string from the end
 * 
 * Now we reconstruct the number backwards.
 * 
 * We reset nb again to the absolute value of n (as long):
 * If original n was 0 → handle as special case: string is "0"
 * 
 * Else:
 * Put the string terminator at the end: str[len] = '\0'
 * Start from index i = len - 1
 * While nb > 0:
 * last_digit = nb % 10 (gives 0–9)
 * convert digit to char: '0' + last_digit
 * place at str[i]
 * i--
 * nb /= 10
 * If is_negative, put '-' at str[0]
 * 
 * Example:
 * With n = -123:
 * 
 * nb = -123 → negative → is_negative = 1 → nb = 123
 * digits = 3
 * len = 4 (3 digits + 1 sign)
 * len = 4 (3 digits + 1 sign)
 * allocate 5 bytes (4 chars + \0)
 * 
 * fill:
 * Index progression:
 * str[4] = '\0'
 * i = 3, nb = 123 → digit = 3 → str[3] = '3'
 * nb = 12, i = 2 → digit = 2 → str[2] = '2'
 * nb = 1, i = 1 → digit = 1 → str[1] = '1'
 * nb = 0 → stop
 * 
 * since negative → str[0] = '-'
 * 
 * Result: "-123".
 * 
 * --> ChatGPT's "opinion" on why this is “nice”:
 * No hardcoded limits
 * Works for all ints
 * Clear conceptual flow
 * Single malloc call, no extra copies
 * Complexity is O(number of digits)
 * 
 * This is the one I’d call “best” for libft: clear, 
 * safe, portable enough, and norm-friendly.
 */

/* ************************************************************************** */

/* Alternative approach B (2 of 5):  All-negative route (no long)
 *
 * ChatGPT thought this was "fun" or "elegant", but if I'm not wrong real
 * use case is only when using 32-bit operations only embedded systems
 * with no long support.
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
 *
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
 *
 * PROS OF METHOD:
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

/* ************************************************************************** */

/* Alternative approach C (3 of 5):
 *
 */

/* ************************************************************************** */

/* Alternativee approach D (4 of 5):
 *
 */

/* ************************************************************************** */

/* Alternative approach E (5 of 5):
 *
 */

/* ************************************************************************** */
/* ************************************************************************** */
/* ************************************************************************** */

/*
 * DISCUSSION:
 *
 * Let's consider "real-life" compiler -O2 optimisation, even though 42 only
 * compiles using cc (Clang nowadays) with warning flags.
 *
 * Compilers (Clang/ LLVM, GCC) optimize:
 * 
 * -> Approach A:
 * Highly optimized
 * "Obvious" patterns
 * Can eliminate unnecessary branches
 * Can inline division strategies
 * 
 * -> Approach B:
 * Optimizes well, but
 * Harder to detect patterns
 * Negative modulo prevents some peephole optimizations
 * 
 * -> Approach C:
 * Compilers optimize the inner loop aggressively
 * Stack buffer often unrolled
 * Very fast
 * 
 * -> Recursive variant:
 * Worst for compilers
 * Limited optimization potential
 * Function calls block aggressive optimization
 */

/* ************************************************************************** */

/*
 * Which approach impresses employers?
 *
 * Clean correctness
 * No undefined behaviour
 * Portability
 * Clear naming & structure
 * Predictability
 * Deep comments showing you understand the internals
 *
 * Approach A wins here.
 */

/* ************************************************************************** */

/*
 * However, What impresses niche employers:
 *
 * Embedded companies → appreciate approach B (no 64-bit math, predictable)
 * HFT / low-latency finance → appreciate approach C (stack buffer, single pass)
 * Aerospace / military → love predictable and portable code (A or B)
 *
 * So:
 * Portfolio GitHub → A
 * Embedded firmware → B
 * Ultra-low latency → C
 *
 * Each approach has a “domain,” but A is the most broadly respected.
 */

/* ************************************************************************** */
/* ************************************************************************** */
/* ************************************************************************** */
/* EOF */
