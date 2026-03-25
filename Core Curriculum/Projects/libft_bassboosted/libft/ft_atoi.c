/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 12:44:41 by hnah              #+#    #+#             */
/*   Updated: 2025/12/08 10:19:22 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
// atoi supposed to return garbage on overflow
int	ft_atoi(const char *nptr)
{
	int	sign;
	int	number;

	number = 0;
	sign = 1;
	while (*nptr == ' ' || *nptr == '\t' || *nptr == '\n'
		|| *nptr == '\r' || *nptr == '\v' || *nptr == '\f')
		nptr++;
	if (*nptr == '-')
	{
		sign = -1;
		nptr++;
	}
	else if (*nptr == '+')
		nptr++;
	while (*nptr && *nptr >= '0' && *nptr <= '9')
	{
		number = number * 10 + (*nptr - '0');
		nptr++;
	}
	return (sign * number);
}

/*
int main(void)
{
    char *s1 = "42";
    char *s2 = "   -42abc";
    char *s3 = "";
    char *s4 = " \t\n\r  42";
    char *s5 = "-00042";
    char *s6 = "2147483647";
    char *s7 = "-2147483648";
    char *s8 = "NULL";

    char *a1 = ft_itoa(atoi(s1));
    char *b1 = ft_itoa(ft_atoi(s1));
    write(1, "1: ", 3); write(1, a1, ft_strlen(a1)); write(1, " | ", 3);
    write(1, b1, ft_strlen(b1)); write(1, "\n", 1);

    char *a2 = ft_itoa(atoi(s2));
    char *b2 = ft_itoa(ft_atoi(s2));
    write(1, "2: ", 3); write(1, a2, ft_strlen(a2)); write(1, " | ", 3);
    write(1, b2, ft_strlen(b2)); write(1, "\n", 1);

    char *a3 = ft_itoa(atoi(s3));
    char *b3 = ft_itoa(ft_atoi(s3));
    write(1, "3: ", 3); write(1, a3, ft_strlen(a3)); write(1, " | ", 3);
    write(1, b3, ft_strlen(b3)); write(1, "\n", 1);

    char *a4 = ft_itoa(atoi(s4));
    char *b4 = ft_itoa(ft_atoi(s4));
    write(1, "4: ", 3); write(1, a4, ft_strlen(a4)); write(1, " | ", 3);
    write(1, b4, ft_strlen(b4)); write(1, "\n", 1);

    char *a5 = ft_itoa(atoi(s5));
    char *b5 = ft_itoa(ft_atoi(s5));
    write(1, "5: ", 3); write(1, a5, ft_strlen(a5)); write(1, " | ", 3);
    write(1, b5, ft_strlen(b5)); write(1, "\n", 1);

    char *a6 = ft_itoa(atoi(s6));
    char *b6 = ft_itoa(ft_atoi(s6));
    write(1, "6: ", 3); write(1, a6, ft_strlen(a6)); write(1, " | ", 3);
    write(1, b6, ft_strlen(b6)); write(1, "\n", 1);

    char *a7 = ft_itoa(atoi(s7));
    char *b7 = ft_itoa(ft_atoi(s7));
    write(1, "7: ", 3); write(1, a7, ft_strlen(a7)); write(1, " | ", 3);
    write(1, b7, ft_strlen(b7)); write(1, "\n", 1);

    char *a8 = ft_itoa(atoi(s8));
    char *b8 = ft_itoa(ft_atoi(s8));
    write(1, "8: ", 3); write(1, a8, ft_strlen(a8)); write(1, " | ", 3);
    write(1, b8, ft_strlen(b8)); write(1, "\n", 1);

    free(a1); free(b1);
    free(a2); free(b2);
    free(a3); free(b3);
    free(a4); free(b4);
    free(a5); free(b5);
    free(a6); free(b6);
    free(a7); free(b7);
    free(a8); free(b8);

    return 0;
}
main tested
 */
