/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 15:35:38 by hnah              #+#    #+#             */
/*   Updated: 2025/12/07 18:04:37 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t	copied;
	size_t	len_src;
	size_t	len_dst;

	len_dst = 0;
	len_src = 0;
	while (len_dst < size && dst[len_dst])
		len_dst++;
	while (src[len_src])
		len_src++;
	if (len_dst >= size)
		return (size + len_src);
	copied = 0;
	while (src[copied] && (len_dst + copied + 1 < size))
	{
		dst[len_dst + copied] = src[copied];
		copied++;
	}
	dst[len_dst + copied] = '\0';
	return (len_dst + len_src);
}

int	main(void)
{
	char		dst_1[50] = "Hello";
	char        dst_2[50] = "Hello";
	char        dst_3[50] = "Hello";
	const char	*src;
	size_t		ret;

	src = " World";
	ret = ft_strlcat(dst_1, src, 12);
	write(1, "\nResult dst: ", 13);
	write(1, dst_1, ft_strlen(dst_1));
	write(1, "\nReturn val: ", 13);
	write(1, ft_itoa(ret), ft_atoi(ft_itoa(ret)));
	ret = ft_strlcat(dst_2, src, 3);
	write(1, "\nResult dst: ", 13);
	write(1, dst_2, ft_strlen(dst_2));
	write(1, "\nReturn val: ", 13);
	write(1, ft_itoa(ret),ft_atoi(ft_itoa(ret)));
	ret = ft_strlcat(dst_3, src, 8);
	write(1, "\nResult dst: ", 13);
	write(1, dst_3, ft_strlen(dst_3));
	write(1, "\nReturn val: ", 13);
	write(1, ft_itoa(ret), ft_atoi(ft_itoa(ret)));
}
