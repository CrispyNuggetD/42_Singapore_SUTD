NAME = libft.a
CC = cc
CFLAGS = -Wall -Wextra -Werror
AR = ar
ARFLAGS = rcs
OBJ_DIR = obj

BASE_SRCS = $(wildcard 0_libft/*.c)
PRINTF_SRCS = $(filter-out %_bonus.c 1_ft_printf/ft_printf_parse_specs.c \
	1_ft_printf/ft_printf_print_config.c 1_ft_printf/ft_printf_printer.c,\
	$(wildcard 1_ft_printf/ft_printf*.c)) \
	1_ft_printf/ft_printf_parse_specs_bonus.c \
	1_ft_printf/ft_printf_print_config_bonus.c \
	1_ft_printf/ft_printf_printer_bonus.c
GNL_SRCS = 1_get_next_line/get_next_line.c \
	1_get_next_line/get_next_line_utils.c
WIP_SRCS = ryker_ft/file_unique/ryker_ft_create_open_unique_file_in_dir.c
CUSTOM_SRCS = $(filter-out ryker_ft/wip/% $(WIP_SRCS),\
	$(wildcard ryker_ft/*.c ryker_ft/*/*.c))
SRCS = $(BASE_SRCS) $(PRINTF_SRCS) $(GNL_SRCS) $(CUSTOM_SRCS)
OBJS = $(addprefix $(OBJ_DIR)/,$(SRCS:.c=.o))
HEADERS = ryker_libft.h $(wildcard 0_libft/*.h 1_ft_printf/*.h 1_get_next_line/*.h \
	ryker_ft/*.h ryker_ft/*/*.h)
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
