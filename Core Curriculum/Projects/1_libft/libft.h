/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 10:30:18 by hnah              #+#    #+#             */
/*   Updated: 2025/12/08 12:36:20 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H
# include <stdlib.h>
# include <unistd.h>

int	ft_isalpha(int c);
int ft_isdigit(int c);
int ft_alnum(int c);
int ft_isascii(int c);
size_t	ft_strlen(const char *s);
size_t	ft_strlcat(char *dst, const char *src, size_t size);
/* FILL IN MISSING FUNCTIONS HERE */
int	ft_atoi(const char *nptr);
char	*ft_itoa(int n);

#endif
