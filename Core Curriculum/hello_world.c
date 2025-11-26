/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hello_world.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 11:38:37 by hnah              #+#    #+#             */
/*   Updated: 2025/11/26 11:41:14 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// created 11:13am
// Stdheader appends time created OF Stdheader.
// Stdheader changes Updated to last save time of Vim.

# include <unistd.h>

int     main()
{
        write(1, "Hello World!\n", 13);
        return 0;
}
