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

typedef void    (*t_handler)(void *context);
typedef struct  s_spec
{
    unsigned char   flags;
    int             width;
    int             precision;
    char            conversion;
}                   t_spec;

void	print_d_i(void);
int 	ft_printf(const char *key, ...);

#endif