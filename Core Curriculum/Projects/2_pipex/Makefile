NAME := pipex
CC := cc
CFLAGS := -Wall -Wextra -Werror

SRC_DIR := src
OBJ_DIR := obj
INC_DIR := includes
LIBFT_DIR := libft/0_libft

SRCS := main.c fds.c process.c execute.c path.c memory.c
OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)
BONUS_SRCS := main_bonus.c pipeline_bonus.c wait_bonus.c heredoc_bonus.c \
	heredoc_utils_bonus.c get_next_line_bonus.c execute.c path.c memory.c
BONUS_OBJS := $(BONUS_SRCS:%.c=$(OBJ_DIR)/%.o)
LIBFT := $(LIBFT_DIR)/libft.a
INCLUDES := -I$(INC_DIR) -I$(LIBFT_DIR) -Ilibft/1_ft_printf -I$(LIBFT_DIR)/ryker_ft
MANDATORY_MARKER := $(OBJ_DIR)/.mandatory
BONUS_MARKER := $(OBJ_DIR)/.bonus

all: $(NAME)

$(NAME): $(MANDATORY_MARKER)

$(MANDATORY_MARKER): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME)
	rm -f $(BONUS_MARKER)
	touch $(MANDATORY_MARKER)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/pipex.h $(LIBFT_DIR)/ryker_ft/ryker_ft.h \
\t$(LIBFT_DIR)/ryker_ft/printf_fd/ryker_ft_printf.h
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BONUS_OBJS): $(INC_DIR)/pipex_bonus.h

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
