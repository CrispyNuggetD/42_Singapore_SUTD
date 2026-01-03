/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 13:22:29 by hnah              #+#    #+#             */
/*   Updated: 2026/01/03 07:35:16 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>
# include "libft/libft.h"
# include <limits.h>

# define FLAG_MINUS 1
# define FLAG_ZERO  2
# define FLAG_PREC  4
# define FLAG_HASH  8
# define FLAG_SPACE 16
# define FLAG_PLUS  32

typedef struct s_spec
{
	unsigned char	flags;
	int				width;
	int				precision;
	char			conversion;
}					t_spec;

typedef struct s_context
{
	va_list	*input;
	t_spec	*spec;
}			t_context;

typedef struct  s_print
{
	char		sign;
	const char	*core;
	size_t		core_len;
	const char	*prefix;
	size_t		prefix_len;
	size_t		prec_zeros;
	size_t		pad_len;
	char		pad_char;
}				t_print;

typedef int	(*t_handler)(t_context *context);

int	ft_printf(const char *key, ...);
int	ft_printf_parse_specs(t_spec *spec, const char **ptr);
int	ft_printf_error_end_stream(va_list *input);
int	ft_printf_d_i(t_context *context);
int	ft_printf_percent(t_context *context);

#endif