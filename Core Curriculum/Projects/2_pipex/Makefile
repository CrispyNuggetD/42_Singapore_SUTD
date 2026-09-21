NAME := pipex
CC := cc
CFLAGS := -Wall -Wextra -Werror

SRC_DIR := src
OBJ_DIR := obj
INC_DIR := includes
LIBFT_DIR := ryker_libft

COMMON_SRCS := fd_open_utils.c usage_printer.c pipeline_handler.c execute.c \
	path_checker.c path_candidate.c cleanup_and_errors.c
SRCS := main.c $(COMMON_SRCS)
OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)
BONUS_ONLY_SRCS := main_bonus.c heredoc_helpers_bonus.c heredoc_bonus.c \
	fd_open_utils_bonus.c
BONUS_SRCS := $(BONUS_ONLY_SRCS) $(COMMON_SRCS)
BONUS_OBJS := $(BONUS_SRCS:%.c=$(OBJ_DIR)/%.o)
BONUS_ONLY_OBJS := $(BONUS_ONLY_SRCS:%.c=$(OBJ_DIR)/%.o)
LIBFT_HEADERS := $(LIBFT_DIR)/ryker_libft.h \
	$(filter-out $(LIBFT_DIR)/ryker_ft/wip/%,\
	$(wildcard $(LIBFT_DIR)/0_libft/*.h $(LIBFT_DIR)/1_ft_printf/*.h \
	$(LIBFT_DIR)/1_get_next_line/*.h $(LIBFT_DIR)/ryker_ft/*.h \
	$(LIBFT_DIR)/ryker_ft/*/*.h))
LIBFT := $(LIBFT_DIR)/libft.a
INCLUDES = -I$(INC_DIR)
MANDATORY_MARKER := $(OBJ_DIR)/.mandatory
BONUS_MARKER := $(OBJ_DIR)/.bonus

all: $(NAME)

$(NAME): $(MANDATORY_MARKER)

$(MANDATORY_MARKER): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME)
	rm -f $(BONUS_MARKER)
	touch $(MANDATORY_MARKER)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/pipex.h $(LIBFT_HEADERS) Makefile
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BONUS_ONLY_OBJS): $(INC_DIR)/pipex_bonus.h


bonus: $(BONUS_MARKER)

$(BONUS_MARKER): $(BONUS_OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(BONUS_OBJS) $(LIBFT) -o $(NAME)
	rm -f $(MANDATORY_MARKER)
	touch $(BONUS_MARKER)

$(LIBFT): FORCE
	$(MAKE) -C $(LIBFT_DIR)

clean:
	rm -rf $(OBJ_DIR)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

FORCE:

.PHONY: all clean fclean re bonus FORCE
