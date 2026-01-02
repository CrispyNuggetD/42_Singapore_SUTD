/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 13:22:29 by hnah              #+#    #+#             */
/*   Updated: 2025/12/26 10:23:01 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>
# include "libft/libft.h"

# define FLAG_MINUS (1 << 0)
# define FLAG_ZERO  (1 << 1)
# define FLAG_HASH  (1 << 2)
# define FLAG_SPACE (1 << 3)
# define FLAG_PLUS  (1 << 4)
# define FLAG_PREC  (1 << 5)

typedef struct  s_spec
{
    unsigned char   flags;
    int             width;
    int             precision;
    char            conversion;
}                   t_spec;

typedef struct s_context
{
    va_list *input;
    t_spec  *spec;
}           t_context;

typedef int (*t_handler)(t_context *context);

int print_d_i(t_context *context);
int parse_spec(t_spec *spec, const char **ptr);
int ft_printf(const char *key, ...);

#endif

