/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 11:37:33 by hnah              #+#    #+#             */
/*   Updated: 2025/11/27 09:43:23 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>


/*
Compilers (LLVM, GCC) optimize:

-> Approach A:
Highly optimized
"Obvious" patterns
Can eliminate unnecessary branches
Can inline division strategies

-> Approach B:
Optimizes well, but
Harder to detect patterns
Negative modulo prevents some peephole optimizations

-> Approach C:
Compilers optimize the inner loop aggressively
Stack buffer often unrolled
Very fast

-> Recursive variant:
Worst for compilers
Limited optimization potential
Function calls block aggressive optimization
*/