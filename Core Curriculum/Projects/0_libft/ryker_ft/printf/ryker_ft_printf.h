#ifndef RYKER_FT_PRINTF_H
# define RYKER_FT_PRINTF_H

# include "ft_printf.h"
# include <errno.h>
# include <limits.h>

typedef struct s_ryker_output
{
	int	fd;
	int	printed;
}	t_ryker_output;

int					ryker_ft_printf_fd(int fd, const char *format, ...);
int					ryker_ft_write(t_ryker_output *out,
						const char *text, size_t length);
int					ryker_ft_conversion(t_ryker_output *out,
						t_spec *spec, va_list *args);
int					ryker_ft_number(t_ryker_output *out,
						t_spec *spec, va_list *args);

int					ryker_ft_parse(t_spec *spec, const char **format,
						va_list *args);
int					ryker_ft_print(t_ryker_output *out,
						t_spec *spec, t_print *paper);

#endif
