NAME = libft.a
CC = cc
CFLAGS = -Wall -Wextra -Werror
AR = ar
ARFLAGS = rcs
OBJ_DIR = obj

BASE_SRCS = 0_libft/ft_atoi.c \
	0_libft/ft_bzero.c \
	0_libft/ft_calloc.c \
	0_libft/ft_isalnum.c \
	0_libft/ft_isalpha.c \
	0_libft/ft_isascii.c \
	0_libft/ft_isdigit.c \
	0_libft/ft_isprint.c \
	0_libft/ft_itoa.c \
	0_libft/ft_lstadd_back.c \
	0_libft/ft_lstadd_front.c \
	0_libft/ft_lstclear.c \
	0_libft/ft_lstdelone.c \
	0_libft/ft_lstiter.c \
	0_libft/ft_lstlast.c \
	0_libft/ft_lstmap.c \
	0_libft/ft_lstnew.c \
	0_libft/ft_lstsize.c \
	0_libft/ft_memchr.c \
	0_libft/ft_memcmp.c \
	0_libft/ft_memcpy.c \
	0_libft/ft_memmove.c \
	0_libft/ft_memset.c \
	0_libft/ft_putchar_fd.c \
	0_libft/ft_putendl_fd.c \
	0_libft/ft_putnbr_fd.c \
	0_libft/ft_putstr_fd.c \
	0_libft/ft_split.c \
	0_libft/ft_strchr.c \
	0_libft/ft_strdup.c \
	0_libft/ft_striteri.c \
	0_libft/ft_strjoin.c \
	0_libft/ft_strlcat.c \
	0_libft/ft_strlcpy.c \
	0_libft/ft_strlen.c \
	0_libft/ft_strmapi.c \
	0_libft/ft_strncmp.c \
	0_libft/ft_strnstr.c \
	0_libft/ft_strrchr.c \
	0_libft/ft_strtrim.c \
	0_libft/ft_substr.c \
	0_libft/ft_tolower.c \
	0_libft/ft_toupper.c
PRINTF_SRCS = 1_ft_printf/ft_printf.c \
	1_ft_printf/ft_printf_character.c \
	1_ft_printf/ft_printf_d_i.c \
	1_ft_printf/ft_printf_fd_setting.c \
	1_ft_printf/ft_printf_helpers.c \
	1_ft_printf/ft_printf_hex_large.c \
	1_ft_printf/ft_printf_hex_small.c \
	1_ft_printf/ft_printf_percent.c \
	1_ft_printf/ft_printf_pointer.c \
	1_ft_printf/ft_printf_string.c \
	1_ft_printf/ft_printf_unsigned_int.c \
	1_ft_printf/ft_printf_parse_specs_bonus.c \
	1_ft_printf/ft_printf_print_config_bonus.c \
	1_ft_printf/ft_printf_printer_bonus.c
GNL_SRCS = 1_get_next_line/get_next_line.c \
	1_get_next_line/get_next_line_utils.c
# Only these custom helpers are built; experimental helpers stay excluded.
CUSTOM_SRCS = ryker_ft/ryker_ft_free_str_array.c \
	ryker_ft/ryker_ft_isspace.c \
	ryker_ft/file_unique/ryker_ft_create_open_unique_file.c \
	ryker_ft/gnl_status/ryker_ft_get_next_line.c \
	ryker_ft/math/ryker_ft_abs.c \
	ryker_ft/math/ryker_ft_array_max_at.c \
	ryker_ft/math/ryker_ft_max.c \
	ryker_ft/math/ryker_ft_min.c \
	ryker_ft/math/ryker_ft_sign.c \
	ryker_ft/math/ryker_ft_update_max.c \
	ryker_ft/math/ryker_ft_update_min.c \
	ryker_ft/printf_fd/ryker_ft_printf_fd.c
SRCS = $(BASE_SRCS) $(PRINTF_SRCS) $(GNL_SRCS) $(CUSTOM_SRCS)
OBJS = $(addprefix $(OBJ_DIR)/,$(SRCS:.c=.o))
HEADERS = ryker_libft.h \
	0_libft/libft.h \
	1_ft_printf/ft_printf.h \
	1_get_next_line/get_next_line.h \
	1_get_next_line/get_next_line_bonus.h \
	ryker_ft/ryker_ft.h \
	ryker_ft/file_unique/ryker_ft_file_unique.h \
	ryker_ft/gnl_status/ryker_ft_get_next_line.h \
	ryker_ft/printf_fd/ryker_ft_printf.h
INCLUDES = -I0_libft -I1_ft_printf -I1_get_next_line -Iryker_ft

all: $(NAME)

$(NAME): $(OBJS) Makefile
	rm -f $(NAME)
	$(AR) $(ARFLAGS) $(NAME) $(OBJS)

$(OBJ_DIR)/%.o: %.c $(HEADERS) Makefile
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
