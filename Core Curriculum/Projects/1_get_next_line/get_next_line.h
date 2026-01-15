/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   get_next_line.h									:+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: hnah <hnah@student.42singapore.sg>		 +#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2025/12/25 13:22:29 by hnah			  #+#	#+#			 */
/*   Updated: 2026/01/09 18:58:17 by hnah			 ###   ########.fr	   */
/*																			*/
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>
# include <limits.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdint.h>

char  *get_next_line(int fd);

#endif
